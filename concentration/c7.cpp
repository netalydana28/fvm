//Navier Stokes on unstructured mesh optimization 
//Taking into account concentration 
#include <bits/stdc++.h>  
using namespace std;
const int node_num=3875, center_num=7386;
double nodes[node_num][2], centers[center_num][2], 
    P_nodes[node_num], U_nodes[node_num], 
    V_nodes[node_num], C_nodes[node_num],
    P_centers_old[center_num], P_centers_new[center_num], 
    U_centers_old[center_num], U_centers_new[center_num], 
    V_centers_old[center_num], V_centers_new[center_num], 
    C_centers_old[center_num], C_centers_new[center_num], f[center_num], 
    eps=0.0001, nyu = 1.0/400.0, dt = 0.00000001, ro = 1.0, d = 1.0/800.0, max_diff, 
    f_volume[center_num], lim_x=1.0, right_lim = lim_x/2.0 + 0.1*lim_x, left_lim = lim_x/2.0 - 0.1*lim_x;
int face_num, l_cell_to_face[center_num][3], l_cell_to_node[center_num][3];
vector<vector<int> > l_face_to_cell, l_face_to_node;
vector<int> node_to_cell[node_num], boundary_face;
vector<vector<double> > sn, f_centers;
vector<double> face_areas, P_faces, U_faces, V_faces, C_faces;
vector<double> split_double(string line, char delimeter){
    string my_str = "";
    vector<double> v;
    for(int i=0; i<line.size(); i++){
        if(((line[i] == delimeter) or (i==line.size()-1))){
            if(line[i+1] == delimeter){
                continue;
            }
            if(i==line.size()-1){
                my_str+=line[i];  
            }
            char* temp = new char[my_str.length()+1];
            strcpy(temp, my_str.c_str());
            double to_insert = atof(temp);
            v.push_back(to_insert);
            my_str = "";
        }else{
            my_str+=line[i];  
        }
    }
    return v;
}
vector<int> split_int(string line, char delimeter){
    string my_str = "";
    vector<int> v;
    for(int i=0; i<line.size(); i++){
        if((line[i] == delimeter) or (i==line.size()-1)){
            if(line[i+1] == delimeter){
                continue;
            }
            if(i==line.size()-1){
                my_str+=line[i];  
            }
            if(my_str[0]== 't'){
                my_str = "";
                continue;
            }
            char* temp = new char[my_str.length()+1];
            strcpy(temp, my_str.c_str());
            int to_insert = atof(temp);
            v.push_back(to_insert);
            my_str = "";
        }else{
            my_str+=line[i];  
        }
    }
    return v;
}
void nodes_coor(){
    string myText;
    ifstream NodesFile("obst9_1_1_coor.txt");
    vector<double> v; 
    int cnt = 0;
    while (getline(NodesFile, myText)) {
        v = split_double(myText, ' ');
        v.erase(v.begin());
        v.erase(v.end()-1);
        nodes[cnt][0] = v[0];
        nodes[cnt][1] = v[1];
        v.clear();
        cnt++;
    }
    NodesFile.close();
}
void cell_to_node(){
    string myText;
    ifstream NodesFile("obst9_1_1_links.txt");
    vector<int> v; 
    while (getline(NodesFile, myText)) {
        v = split_int(myText, ' ');
        int cell = v[0];
        v.erase(v.begin());
        v.erase(v.begin());
        for(int i=0; i<3; i++){
            l_cell_to_node[cell-1][i] = v[i];
            node_to_cell[v[i]-1].push_back(cell);
        }
        v.clear();
    }
    NodesFile.close();
}
void center_coordinates_generator(){
    for(int i = 0; i < center_num; i++){
        int node1 = l_cell_to_node[i][0], node2 = l_cell_to_node[i][1], node3 = l_cell_to_node[i][2];
        centers[i][0] = ((nodes[node1-1][0] + nodes[node2-1][0] + nodes[node3-1][0]) / 3);
        centers[i][1] = ((nodes[node1-1][1] + nodes[node2-1][1] + nodes[node3-1][1]) / 3);
    }
}
void face_links(){
    map< set<int>, vector<int> > nodes_to_cell;
    vector<int> key1, trg_nodes;
    int cnt = 1;
    set<int> nodes_ind;
    vector< vector<int> > c_t_f(center_num);
    for(int i=1; i<center_num+1; i++){
        for(int j=1; j<4; j++){
            register int trg_node = l_cell_to_node[i-1][j-1];
            trg_nodes.push_back(trg_node);
        }
        register int m;
        for(int j=1; j<4; j++){
            if(j==3){
                m = 0;
            }else{
                m = j;
            }
            nodes_ind.insert(trg_nodes[j-1]); 
            nodes_ind.insert(trg_nodes[m]);
            if(nodes_to_cell.find(nodes_ind) == nodes_to_cell.end()){
                key1.push_back(i);
                nodes_to_cell[nodes_ind] = key1; 
                key1.clear(); 
            }else{
                nodes_to_cell[nodes_ind].push_back(i);
            }
            nodes_ind.clear();           
        }
        trg_nodes.clear();
    }
    face_num = nodes_to_cell.size();
    map< set<int>, vector<int> >::iterator m_it=nodes_to_cell.begin();
    register int f_cnt = 1;
    while(m_it!=nodes_to_cell.end()){
        key1 = m_it->second; 
        set <int>::iterator s_it = m_it->first.begin();
        int s_cnt = 0;
        vector<int> temp;
        while(s_it != m_it->first.end()){
            temp.push_back(*s_it);
            s_cnt++;
            s_it++;
        }
        l_face_to_node.push_back(temp);
        temp.clear();
        for(int i=0; i<key1.size(); i++){
            c_t_f[key1[i]-1].push_back(f_cnt);
            temp.push_back(key1[i]); 
            if(key1.size() == 1){
                boundary_face.push_back(f_cnt);
                temp.push_back(key1[i]); 
                continue;
            }   
        }
        l_face_to_cell.push_back(temp);
        temp.clear();
        f_cnt++;

        m_it++;
        key1.clear();
    }
    for(int i=0; i<center_num; i++){
        for(int j=0; j<c_t_f[i].size(); j++){
            l_cell_to_face[i][j] = c_t_f[i][j];
        }
    }
    for(int i=0; i<center_num; i++){
        f[i] = 0;
    }
    cout<<l_face_to_cell.size()<<endl;
    cout<<l_face_to_node.size()<<endl;
}
vector<double> face_center(int face_ind){
    double n1[2], n2[2];
    n1[0] = nodes[l_face_to_node[face_ind-1][0]-1][0];
    n1[1] = nodes[l_face_to_node[face_ind-1][0]-1][1];
    n2[0] = nodes[l_face_to_node[face_ind-1][1]-1][0];
    n2[1] = nodes[l_face_to_node[face_ind-1][1]-1][1];    
    vector<double> ans; 
    ans.push_back((n1[0] + n2[0])/2);
    ans.push_back((n1[1] + n2[1])/2);
    return ans;
}
vector<double> normal(int face_ind){
    vector <double> ans; 
    double n1[2], n2[2];
    n1[0] = nodes[l_face_to_node[face_ind-1][0]-1][0];
    n1[1] = nodes[l_face_to_node[face_ind-1][0]-1][1];
    n2[0] = nodes[l_face_to_node[face_ind-1][1]-1][0];
    n2[1] = nodes[l_face_to_node[face_ind-1][1]-1][1];
    ans.push_back(n1[1] - n2[1]); 
    ans.push_back(n2[0] - n1[0]);  
    double len = pow(pow(ans[0], 2) + pow(ans[1], 2), 0.5); 
    for(int i=0; i<ans.size(); i++){
        ans[i] /= len;
    }
    return ans; 
}
double area(int face_ind){
    double n1[2], n2[2];
    n1[0] = nodes[l_face_to_node[face_ind-1][0]-1][0];
    n1[1] = nodes[l_face_to_node[face_ind-1][0]-1][1];
    n2[0] = nodes[l_face_to_node[face_ind-1][1]-1][0];
    n2[1] = nodes[l_face_to_node[face_ind-1][1]-1][1];
    return pow((pow((n1[0] - n2[0]), 2) + pow((n1[1] - n2[1]), 2)) , 0.5);
}
double volume(int cen_ind){
    int n1 = l_cell_to_node[cen_ind-1][0], n2 = l_cell_to_node[cen_ind-1][1], n3 = l_cell_to_node[cen_ind-1][2];
    double x1 = nodes[n1-1][0], x2 = nodes[n2-1][0], x3 = nodes[n3-1][0], 
        y1 = nodes[n1-1][1], y2 = nodes[n2-1][1], y3 = nodes[n3-1][1];
    return (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2;
}
void cell_to_vertex_interpolation_old(){ 
    for(int node_ind=1; node_ind<node_num+1; node_ind++){
        double node_coor[2], center_coor[2], nom_U = 0, denom = 0, nom_V = 0, nom_P = 0, nom_C = 0, P, V, U, C, d;  
        node_coor[0]= nodes[node_ind-1][0]; 
        node_coor[1]= nodes[node_ind-1][1];  
        int cell_ind; 
        for(int i=0; i<node_to_cell[node_ind-1].size(); i++){ 
            cell_ind = node_to_cell[node_ind-1][i]; 
            center_coor[0] = centers[cell_ind-1][0]; 
            center_coor[1] = centers[cell_ind-1][1]; 
            d = pow(pow((center_coor[0] - node_coor[0]), 2) + pow((center_coor[1] - node_coor[1]), 2), 0.5); 
            P = P_centers_old[cell_ind - 1],  U = U_centers_old[cell_ind - 1], 
                V = V_centers_old[cell_ind - 1], C = C_centers_old[cell_ind - 1]; 
            nom_P += P/d;
            nom_U += U/d; 
            nom_V += V/d; 
            nom_C += C/d; 
            denom += 1/d; 
        } 
        U_nodes[node_ind-1] = nom_U/denom;
        V_nodes[node_ind-1] = nom_V/denom;
        P_nodes[node_ind-1] = nom_P/denom;
        C_nodes[node_ind-1] = nom_C/denom;
    }
}
void cell_to_vertex_interpolation_new(){ 
    for(int node_ind=1; node_ind<node_num+1; node_ind++){
        double node_coor[2], center_coor[2], nom_U = 0, denom = 0, nom_V = 0, nom_P = 0, nom_C = 0, P, U, V, C, d; 
        node_coor[0]= nodes[node_ind-1][0]; 
        node_coor[1]= nodes[node_ind-1][1]; 
        int cell_ind; 
        for(int i=0; i<node_to_cell[node_ind-1].size(); i++){ 
            cell_ind = node_to_cell[node_ind-1][i]; 
            center_coor[0] = centers[cell_ind-1][0]; 
            center_coor[1] = centers[cell_ind-1][1]; 
            d = pow(pow((center_coor[0] - node_coor[0]), 2) + pow((center_coor[1] - node_coor[1]), 2), 0.5); 
            P = P_centers_new[cell_ind - 1], U = U_centers_new[cell_ind - 1], 
                V = V_centers_new[cell_ind - 1], C = C_centers_new[cell_ind - 1]; 
            nom_P += P/d;
            nom_U += U/d; 
            nom_V += V/d; 
            nom_C += C/d; 
            denom += 1/d; 
        } 
        U_nodes[node_ind-1] = nom_U/denom;
        V_nodes[node_ind-1] = nom_V/denom;
        P_nodes[node_ind-1] = nom_P/denom;
        C_nodes[node_ind-1] = nom_C/denom;
    }
}
void set_boundary_conditions_C(){
    double x1, x2, nx, ny, area, dx, dy, center_coor[2], nom, denom, valid_denom, y1, y2; int face_ind, fn1,fn2, b_f, n[2];
    vector<double> face_coor;
    for(int l=0; l<boundary_face.size(); l++){
        b_f = boundary_face[l];
        n[0] = l_face_to_node[b_f-1][0];
        x1 = nodes[n[0]-1][0], y1 = nodes[n[0]-1][1]; 
        n[1] = l_face_to_node[b_f-1][1];
        x2 = nodes[n[1]-1][0], y2 = nodes[n[1]-1][1];  
        if(((x1==0.0) and(x2==0.0)) or ((y1==0.0) and (y2==0.0)) and ((y1==lim_x) and (y2==lim_x))){
            C_nodes[n[0]-1] = 0;
            C_nodes[n[1]-1] = 0;
        }else if(x1==x2){
            for(int k = 0; k<2; k++){
                for(int i=0; i<node_to_cell[n[k]-1].size(); i++){
                    nom = 0, denom = 0;
                    center_coor[0] = centers[node_to_cell[n[k]-1][i]-1][0];
                    center_coor[1] = centers[node_to_cell[n[k]-1][i]-1][1];
                    for(int j=1; j<4; j++){
                        face_ind = l_cell_to_face[node_to_cell[n[k]-1][i]-1][j-1];
                        fn1 = l_face_to_node[face_ind-1][0], fn2 = l_face_to_node[face_ind-1][1];
                        if(fn2 == n[k]){
                            fn2 = fn1; 
                        }else if ((fn2 != n[k]) and (fn1 != n[k])){
                            continue;
                        }
                        face_coor = f_centers[face_ind-1];
                        nx = face_coor[1] - center_coor[1], ny = center_coor[0] - face_coor[0];
                        nx /= pow(pow(nx, 2) + pow(ny, 2), 0.5);
                        area = pow(pow(face_coor[0] - center_coor[0], 2) + pow(face_coor[1] - center_coor[1], 2), 0.5);
                        dx = nodes[fn2-1][0] - nodes[n[k]-1][0];
                        if(((dx>0) and (nx<0)) or ((dx<0) and (nx>0))){
                            nx*=(-1);
                        }
                        valid_denom=0;
                        if(dx != 0){
                            valid_denom += (nx/dx);
                        }
                        nom += C_nodes[fn2-1] * area * valid_denom;
                        denom += area * valid_denom;
                    }
                    if(denom!=0){
                        C_nodes[n[k]-1] = nom/denom;
                    }else{
                        C_nodes[n[k]-1] = 0;
                    }
                }
            }
        }else if (y1==y2){
             for(int k = 0; k<2; k++){
                for(int i=0; i<node_to_cell[n[k]-1].size(); i++){
                    nom = 0, denom = 0;
                    center_coor[0] = centers[node_to_cell[n[k]-1][i]-1][0];
                    center_coor[1] = centers[node_to_cell[n[k]-1][i]-1][1];
                    for(int j=1; j<4; j++){
                        face_ind = l_cell_to_face[node_to_cell[n[k]-1][i]-1][j-1];
                        fn1 = l_face_to_node[face_ind-1][0], fn2 = l_face_to_node[face_ind-1][1];
                        if(fn2 == n[k]){
                            fn2 = fn1; 
                        }else if ((fn2 != n[k]) and (fn1 != n[k])){
                            continue;
                        }
                        face_coor = f_centers[face_ind-1];
                        nx = face_coor[1] - center_coor[1], ny = center_coor[0] - face_coor[0];
                        nx /= pow(pow(nx, 2) + pow(ny, 2), 0.5);
                        ny /= pow(pow(nx, 2) + pow(ny, 2), 0.5);
                        area = pow(pow(face_coor[0] - center_coor[0], 2) + pow(face_coor[1] - center_coor[1], 2), 0.5);
                        dy = nodes[fn2-1][1] - nodes[n[k]-1][1];
                        if(((dy>0) and (ny<0)) or ((dy<0) and (ny>0))){
                            ny*=(-1);
                        }
                        valid_denom=0;
                        if(dy != 0){
                            valid_denom += (ny/dy);
                        }
                        nom += C_nodes[fn2-1] * area * valid_denom;
                        denom += area * valid_denom;
                    }
                    if(denom!=0){
                        C_nodes[n[k]-1] = nom/denom;
                    }else{
                        C_nodes[n[k]-1] = 0;
                    }
                }
            }
        }
        if((x1==0.0) and (x2==0.0) and (y1>=left_lim) and (y1<=right_lim) and (y2>=left_lim) and (y2<=right_lim)){
            C_nodes[n[0]-1] = 1;
            C_nodes[n[1]-1] = 1;
        }
        C_faces[b_f-1] = (C_nodes[n[0]-1] + C_nodes[n[1]-1])/2;
    }
}
void set_boundary_conditions_U(){
    double x1, x2, nx, ny, area, dx, center_coor[2], nom, denom, valid_denom, y1, y2; int face_ind, fn1,fn2, b_f, n[2];
    vector <double> face_coor;
    for(int l=0; l<boundary_face.size(); l++){
        b_f = boundary_face[l];
        n[0] = l_face_to_node[b_f-1][0];
        x1 = nodes[n[0]-1][0], y1 = nodes[n[0]-1][1]; 
        n[1] = l_face_to_node[b_f-1][1];
        x2 = nodes[n[1]-1][0], y2 = nodes[n[1]-1][1];    
        if((x1==lim_x) and (x2==lim_x)){
            for(int k = 0; k<2; k++){
                for(int i=0; i<node_to_cell[n[k]-1].size(); i++){
                    nom = 0, denom = 0;
                    center_coor[0] = centers[node_to_cell[n[k]-1][i]-1][0];
                    center_coor[1] = centers[node_to_cell[n[k]-1][i]-1][1];
                    for(int j=1; j<4; j++){
                        face_ind = l_cell_to_face[node_to_cell[n[k]-1][i]-1][j-1];
                        fn1 = l_face_to_node[face_ind-1][0], fn2 = l_face_to_node[face_ind-1][1];
                        if(fn2 == n[k]){
                            fn2 = fn1; 
                        }else if ((fn2 != n[k]) and (fn1 != n[k])){
                            continue;
                            
                        }
                        face_coor = f_centers[face_ind-1];
                        nx = face_coor[1] - center_coor[1], ny = center_coor[0] - face_coor[0];
                        nx /= pow(pow(nx, 2) + pow(ny, 2), 0.5);
                        area = pow(pow(face_coor[0] - center_coor[0], 2) + pow(face_coor[1] - center_coor[1], 2), 0.5);
                        dx = nodes[fn2-1][0] - nodes[n[k]-1][0];
                        if(((dx>0) and (nx<0)) or ((dx<0) and (nx>0))){
                            nx*=(-1);
                        }
                        valid_denom=0;
                        if(dx != 0){
                            valid_denom += (nx/dx);
                        }
                        nom += U_nodes[fn2-1] * area * valid_denom;
                        denom += area * valid_denom;
                    }
                    if(denom!=0){
                        U_nodes[n[k]-1] = nom/denom;
                    }else{
                        U_nodes[n[k]-1] = 0;
                    }
                }
            }
        }else{
            U_nodes[n[0]-1] = 0;
            U_nodes[n[1]-1] = 0;
        }
        if((x1==0.0) and (x2==0.0) and (y1>=left_lim) and (y1<=right_lim) and (y2>=left_lim) and (y2<=right_lim)){
            U_nodes[n[0]-1] = 1;
            U_nodes[n[1]-1] = 1;
        }
        U_faces[b_f-1] = (U_nodes[n[0]-1] + U_nodes[n[1]-1])/2;
    }
}
void set_boundary_conditions_V(){
    double x1, x2, nx, ny, area, dx, center_coor[2], nom, denom, valid_denom, y1, y2; int face_ind, fn1,fn2, b_f, n[2];
    vector <double> face_coor;
    for(int l=0; l<boundary_face.size(); l++){
        b_f = boundary_face[l];
        n[0] = l_face_to_node[b_f-1][0];
        x1 = nodes[n[0]-1][0], y1 = nodes[n[0]-1][1]; 
        n[1] = l_face_to_node[b_f-1][1];
        x2 = nodes[n[1]-1][0], y2 = nodes[n[1]-1][1];   
        if((x1==lim_x) and (x2==lim_x)){
            for(int k = 0; k<2; k++){
                for(int i=0; i<node_to_cell[n[k]-1].size(); i++){
                    nom = 0, denom = 0;
                    center_coor[0] = centers[node_to_cell[n[k]-1][i]-1][0];
                    center_coor[1] = centers[node_to_cell[n[k]-1][i]-1][1];
                    for(int j=1; j<4; j++){
                        face_ind = l_cell_to_face[node_to_cell[n[k]-1][i]-1][j-1];
                        fn1 = l_face_to_node[face_ind-1][0], fn2 = l_face_to_node[face_ind-1][1];
                        if(fn2 == n[k]){
                            fn2 = fn1; 
                        }else if ((fn2 != n[k]) and (fn1 != n[k])){
                            continue;
                        }
                        face_coor = f_centers[face_ind-1];
                        nx = face_coor[1] - center_coor[1], ny = center_coor[0] - face_coor[0];
                        nx /= pow(pow(nx, 2) + pow(ny, 2), 0.5);
                        area = pow(pow(face_coor[0] - center_coor[0], 2) + pow(face_coor[1] - center_coor[1], 2), 0.5);
                        dx = nodes[fn2-1][0] - nodes[n[k]-1][0];
                        if(((dx>0) and (nx<0)) or ((dx<0) and (nx>0))){
                            nx*=(-1);
                        }
                        valid_denom=0;
                        if(dx != 0){
                            valid_denom += (nx/dx);
                        }
                        nom += V_nodes[fn2-1] * area * valid_denom;
                        denom += area * valid_denom;
                    }
                    if(denom != 0){
                        V_nodes[n[k]-1] = nom/denom;
                    }else{
                        V_nodes[n[k]-1] = 0;
                    }
                }
            }
        }else{
            V_nodes[n[0]-1] = 0;
            V_nodes[n[1]-1] = 0;
        }
        V_faces[b_f-1] = (V_nodes[n[0]-1] + V_nodes[n[1]-1])/2;
    }
}
void set_boundary_conditions_P(){
    vector<int> v;
    double x1, x2, nx, ny, area, dx, dy, center_coor[2], nom, denom, valid_denom, y1, y2; int face_ind, fn1,fn2, b_f, n[2];
    vector<double> face_coor;
    for(int l=0; l<boundary_face.size(); l++){
        b_f = boundary_face[l];
        n[0] = l_face_to_node[b_f-1][0];
        x1 = nodes[n[0]-1][0], y1 = nodes[n[0]-1][1]; 
        n[1] = l_face_to_node[b_f-1][1];
        x2 = nodes[n[1]-1][0], y2 = nodes[n[1]-1][1]; 
        if((x1==0.0) and (x2==0.0)){
            P_nodes[n[0]-1] = 0;
            P_nodes[n[1]-1] = 0;
        }else if(x1==x2){
            for(int k = 0; k<2; k++){
                for(int i=0; i<node_to_cell[n[k]-1].size(); i++){
                    nom = 0, denom = 0;
                    center_coor[0] = centers[node_to_cell[n[k]-1][i]-1][0];
                    center_coor[1] = centers[node_to_cell[n[k]-1][i]-1][1];
                    for(int j=1; j<4; j++){
                        face_ind = l_cell_to_face[node_to_cell[n[k]-1][i]-1][j-1];
                        fn1 = l_face_to_node[face_ind-1][0], fn2 = l_face_to_node[face_ind-1][1];
                        if(fn2 == n[k]){
                            fn2 = fn1; 
                        }else if ((fn2 != n[k]) and (fn1 != n[k])){
                            continue;
                        }
                        face_coor = f_centers[face_ind-1];
                        nx = face_coor[1] - center_coor[1], ny = center_coor[0] - face_coor[0];
                        nx /= pow(pow(nx, 2) + pow(ny, 2), 0.5);
                        ny /= pow(pow(nx, 2) + pow(ny, 2), 0.5);
                        area = pow(pow(face_coor[0] - center_coor[0], 2) + pow(face_coor[1] - center_coor[1], 2), 0.5);
                        dx = nodes[fn2-1][0] - nodes[n[k]-1][0];
                        if(((dx>0) and (nx<0)) or ((dx<0) and (nx>0))){
                            nx*=(-1);
                        }
                        valid_denom=0;
                        if(dx != 0){
                            valid_denom += (nx/dx);
                        }
                        nom += P_nodes[fn2-1] * area * valid_denom;
                        denom += area * valid_denom;
                    }
                    if(denom!=0){
                        P_nodes[n[k]-1] = nom/denom;
                    }else{
                        P_nodes[n[k]-1] = 0;
                    }
                }
            }
        }else if (y1==y2){
            for(int k = 0; k<2; k++){
                for(int i=0; i<node_to_cell[n[k]-1].size(); i++){
                    nom = 0, denom = 0;
                    center_coor[0] = centers[node_to_cell[n[k]-1][i]-1][0];
                    center_coor[1] = centers[node_to_cell[n[k]-1][i]-1][1];
                    for(int j=1; j<4; j++){
                        face_ind = l_cell_to_face[node_to_cell[n[k]-1][i]-1][j-1];
                        fn1 = l_face_to_node[face_ind-1][0], fn2 = l_face_to_node[face_ind-1][1];
                        if(fn2 == n[k]){
                            fn2 = fn1; 
                        }else if ((fn2 != n[k]) and (fn1 != n[k])){
                            continue;
                        }
                        face_coor = f_centers[face_ind-1];
                        nx = face_coor[1] - center_coor[1], ny = center_coor[0] - face_coor[0];
                        nx /= pow(pow(nx, 2) + pow(ny, 2), 0.5);
                        ny /= pow(pow(nx, 2) + pow(ny, 2), 0.5);
                        area = pow(pow(face_coor[0] - center_coor[0], 2) + pow(face_coor[1] - center_coor[1], 2), 0.5);
                        dy = nodes[fn2-1][1] - nodes[n[k]-1][1];
                        if(((dy>0) and (ny<0)) or ((dy<0) and (ny>0))){
                            ny*=(-1);
                        }
                        valid_denom=0;
                        if(dy != 0){
                            valid_denom += (ny/dy);
                        }
                        nom += P_nodes[fn2-1] * area * valid_denom;
                        denom += area * valid_denom;
                    }
                    if(denom!=0){
                        P_nodes[n[k]-1] = nom/denom;
                    }else{
                        P_nodes[n[k]-1] = 0;
                    }
                }
            }
        }
        P_faces[b_f-1] = (P_nodes[n[0]-1] + P_nodes[n[1]-1])/2;
    }
}
bool is_boundary_face(int face_ind){
    if(l_face_to_cell[face_ind-1][0] == l_face_to_cell[face_ind-1][1]){
        return true;
    }
    return false;
}
void fvm(){
    double au, av, ac, center_coor[2], deltx, delty, nx, ny, valid_denom, nom, denom, c2[2];
    register int out_center_ind, face_ind;
    vector<double> face_coor;
    for(int main=0; main < 5.0e+006; main++){
        printf("%d \n", main);
        // double start_time =  clock();
        for(int i=1; i<center_num+1; i++){
            au=0, av=0, ac = 0;
            center_coor[0] = centers[i-1][0];
            center_coor[1] = centers[i-1][1];
            for(int j=1; j<4; j++){
                face_ind = l_cell_to_face[i-1][j-1];
                if(is_boundary_face(face_ind)){   
                    face_coor = f_centers[face_ind-1];
                    deltx = face_coor[0] - center_coor[0];
                    delty = face_coor[1] - center_coor[1];
                    nx = sn[face_ind-1][0];
                    ny = sn[face_ind-1][1];
                    if(((deltx>0) and (nx<0)) or ((deltx<0) and (nx>0))){
                        nx*=(-1);
                    }
                    if(((delty>0) and (ny<0)) or ((delty<0) and (ny>0))){
                        ny*=(-1);
                    }
                    valid_denom=0;
                    if(deltx != 0){
                        valid_denom += (nx/deltx);
                    }
                    if(delty != 0){
                        valid_denom += (ny/delty);
                    }
                    au += face_areas[face_ind-1]*(U_faces[face_ind-1] - U_centers_old[i-1])*((nyu*valid_denom) - U_centers_old[i-1]*nx - V_centers_old[i-1]*ny);
                    av += face_areas[face_ind-1]*(V_faces[face_ind-1] - V_centers_old[i-1])*((nyu*valid_denom) - U_centers_old[i-1]*nx - V_centers_old[i-1]*ny);
                    ac += face_areas[face_ind-1]*(C_faces[face_ind-1] - C_centers_old[i-1])*((d*valid_denom) - U_centers_old[i-1]*nx - V_centers_old[i-1]*ny);
                }else{
                    nx = sn[face_ind-1][0];
                    out_center_ind = l_face_to_cell[face_ind-1][0]; 
                    c2[0] = centers[out_center_ind-1][0];
                    c2[1] = centers[out_center_ind-1][1];
                    ny = sn[face_ind-1][1];
                    if(out_center_ind == i){
                        out_center_ind = l_face_to_cell[face_ind-1][1];
                        c2[0] = centers[out_center_ind-1][0];
                        c2[1] = centers[out_center_ind-1][1];
                    }
                    deltx = c2[0] - center_coor[0];
                    delty = c2[1] - center_coor[1];
                    if(((deltx>0) and (nx<0)) or ((deltx<0) and (nx>0))){
                        nx*=(-1);
                    }
                    if(((delty>0) and (ny<0)) or ((delty<0) and (ny>0))){
                        ny*=(-1);
                    }
                    valid_denom=0;
                    if(deltx != 0){
                        valid_denom += (nx/deltx);
                    }
                    if(delty != 0){
                        valid_denom += (ny/delty);
                    }
                    au += (U_centers_old[out_center_ind-1] - U_centers_old[i-1]) * face_areas[face_ind-1]*((nyu*valid_denom) - U_centers_old[i-1]*nx - V_centers_old[i-1]*ny);
                    av += (V_centers_old[out_center_ind-1] - V_centers_old[i-1]) * face_areas[face_ind-1]*((nyu*valid_denom) - U_centers_old[i-1]*nx - V_centers_old[i-1]*ny);
                    ac += (C_centers_old[out_center_ind-1] - C_centers_old[i-1]) * face_areas[face_ind-1]*((d*valid_denom) - U_centers_old[i-1]*nx - V_centers_old[i-1]*ny);

                }
            }
            U_centers_new[i-1] = U_centers_old[i-1] + (dt/f_volume[i-1]) * au;
            V_centers_new[i-1] = V_centers_old[i-1] + (dt/f_volume[i-1]) * av; 
            C_centers_new[i-1] = C_centers_old[i-1] + (dt/f_volume[i-1]) * ac;
        }
        
        cell_to_vertex_interpolation_new();
        set_boundary_conditions_U();
        set_boundary_conditions_V();
        for(int i=1; i<center_num+1; i++){
            au=0, av=0;
            center_coor[0] = centers[i-1][0];
            center_coor[1] = centers[i-1][1];
            for(int j=1; j<4; j++){
                face_ind = l_cell_to_face[i-1][j-1];
                if(is_boundary_face(face_ind)){   
                    face_coor = f_centers[face_ind-1];
                    deltx = face_coor[0] - center_coor[0];
                    delty = face_coor[1] - center_coor[1];
                    nx = sn[face_ind-1][0];
                    ny = sn[face_ind-1][1];
                    if(((deltx>0) and (nx<0)) or ((deltx<0) and (nx>0))){
                        nx*=(-1);
                    }
                    if(((delty>0) and (ny<0)) or ((delty<0) and (ny>0))){
                        ny*=(-1);
                    }
                    au += (face_areas[face_ind-1])*(U_faces[face_ind-1] - U_centers_new[i-1])*nx;
                    av += (face_areas[face_ind-1])*(V_faces[face_ind-1] - V_centers_new[i-1])*ny;
                }else{
                    nx = sn[face_ind-1][0];
                    out_center_ind = l_face_to_cell[face_ind-1][0]; 
                    c2[0] = centers[out_center_ind-1][0];
                    c2[1] = centers[out_center_ind-1][1];
                    ny = sn[face_ind-1][1];
                    if(out_center_ind == i){
                        out_center_ind = l_face_to_cell[face_ind-1][1];
                        c2[0] = centers[out_center_ind-1][0];
                        c2[1] = centers[out_center_ind-1][1];
                    }
                    deltx = c2[0] - center_coor[0];
                    delty = c2[1] - center_coor[1];
                    if(((deltx>0) and (nx<0)) or ((deltx<0) and (nx>0))){
                        nx*=(-1);
                    }
                    if(((delty>0) and (ny<0)) or ((delty<0) and (ny>0))){
                        ny*=(-1);
                    }
                    au += (U_centers_new[out_center_ind-1] - U_centers_new[i-1]) * face_areas[face_ind-1]*nx;
                    av += (V_centers_new[out_center_ind-1] - V_centers_new[i-1]) * face_areas[face_ind-1]*ny;
                }
            }
            f[i-1] = (ro/dt) * (au + av);
        }
        do{
            cell_to_vertex_interpolation_old();
            set_boundary_conditions_P();
            for(int i=1; i<center_num+1; i++){
                nom=0, denom=0;
                center_coor[0] = centers[i-1][0];
                center_coor[1] = centers[i-1][1];
                for(int j=1; j<4; j++){
                    face_ind = l_cell_to_face[i-1][j-1];
                    if(is_boundary_face(face_ind)){   
                        face_coor = f_centers[face_ind-1];
                        deltx = face_coor[0] - center_coor[0];
                        delty = face_coor[1] - center_coor[1];
                        nx = sn[face_ind-1][0];
                        ny = sn[face_ind-1][1];
                        if(((deltx>0) and (nx<0)) or ((deltx<0) and (nx>0))){
                            nx*=(-1);
                        }
                        if(((delty>0) and (ny<0)) or ((delty<0) and (ny>0))){
                            ny*=(-1);
                        }
                        valid_denom=0;
                        if(deltx != 0){
                            valid_denom += (nx/deltx);
                        }
                        if(delty != 0){
                            valid_denom += (ny/delty);
                        }
                        nom += face_areas[face_ind-1]*P_faces[face_ind-1]*valid_denom;
                        denom += face_areas[face_ind-1]*valid_denom;
                    }else{
                        nx = sn[face_ind-1][0];
                        out_center_ind = l_face_to_cell[face_ind-1][0]; 
                        c2[0] = centers[out_center_ind-1][0];
                        c2[1] = centers[out_center_ind-1][1];
                        ny = sn[face_ind-1][1];
                        if(out_center_ind == i){
                            out_center_ind = l_face_to_cell[face_ind-1][1];
                            c2[0] = centers[out_center_ind-1][0];
                            c2[1] = centers[out_center_ind-1][1];
                        }
                        deltx = c2[0] - center_coor[0];
                        delty = c2[1] - center_coor[1];
                        if(((deltx>0) and (nx<0)) or ((deltx<0) and (nx>0))){
                            nx*=(-1);
                        }
                        if(((delty>0) and (ny<0)) or ((delty<0) and (ny>0))){
                            ny*=(-1);
                        }
                        valid_denom=0;
                        if(deltx != 0){
                            valid_denom += (nx/deltx);
                        }
                        if(delty != 0){
                            valid_denom += (ny/delty);
                        }
                        nom += face_areas[face_ind-1]*valid_denom*P_centers_old[out_center_ind-1];
                        denom += face_areas[face_ind-1]*valid_denom;
                    }
                }
                nom -= f[i-1]*f_volume[i-1];
                P_centers_new[i-1] = nom/denom;
            }
            max_diff = 0.0; 
            for(int i = 0; i< center_num; i++){
                if(max_diff<fabs(P_centers_new[i] - P_centers_old[i])){
                    max_diff = fabs(P_centers_new[i] - P_centers_old[i]);
                }
                P_centers_old[i] = P_centers_new[i];
            }
        }while(max_diff>eps);
        cell_to_vertex_interpolation_new();
        set_boundary_conditions_P();
        for(int i=1; i<center_num+1; i++){
            au=0, av=0;
            center_coor[0] = centers[i-1][0];
            center_coor[1] = centers[i-1][1];
            for(int j=1; j<4; j++){
                face_ind = l_cell_to_face[i-1][j-1];
                if(is_boundary_face(face_ind)){   
                    face_coor = f_centers[face_ind-1];
                    deltx = face_coor[0] - center_coor[0];
                    delty = face_coor[1] - center_coor[1];
                    nx = sn[face_ind-1][0];
                    ny = sn[face_ind-1][1];
                    if(((deltx>0) and (nx<0)) or ((deltx<0) and (nx>0))){
                        nx*=(-1);
                    }
                    if(((delty>0) and (ny<0)) or ((delty<0) and (ny>0))){
                        ny*=(-1);
                    }
                    valid_denom=0;
                    if(deltx != 0){
                        valid_denom += (nx/deltx);
                    }
                    if(delty != 0){
                        valid_denom += (ny/delty);
                    }
                    au += face_areas[face_ind-1]*(P_faces[face_ind-1] - P_centers_new[i-1])*nx;
                    av += face_areas[face_ind-1]*(P_faces[face_ind-1] - P_centers_new[i-1])*ny;
                }else{
                    nx = sn[face_ind-1][0];
                    out_center_ind = l_face_to_cell[face_ind-1][0]; 
                    c2[0] = centers[out_center_ind-1][0];
                    c2[1] = centers[out_center_ind-1][1];
                    ny = sn[face_ind-1][1];
                    if(out_center_ind == i){
                        out_center_ind = l_face_to_cell[face_ind-1][1];
                        c2[0] = centers[out_center_ind-1][0];
                        c2[1] = centers[out_center_ind-1][1];
                    }
                    deltx = c2[0] - center_coor[0];
                    delty = c2[1] - center_coor[1];
                    if(((deltx>0) and (nx<0)) or ((deltx<0) and (nx>0))){
                        nx*=(-1);
                    }
                    if(((delty>0) and (ny<0)) or ((delty<0) and (ny>0))){
                        ny*=(-1);
                    }
                    valid_denom=0;
                    if(deltx != 0){
                        valid_denom += (nx/deltx);
                    }
                    if(delty != 0){
                        valid_denom += (ny/delty);
                    }
                    au += face_areas[face_ind-1]*(P_centers_new[out_center_ind-1] - P_centers_new[i-1])*nx;
                    av += face_areas[face_ind-1]*(P_centers_new[out_center_ind-1] - P_centers_new[i-1])*ny;
                }
            }
            U_centers_old[i-1] = U_centers_new[i-1] - (dt/(ro * f_volume[i-1])) * au;
            V_centers_old[i-1] = V_centers_new[i-1] - (dt/(ro * f_volume[i-1]))* av;
            C_centers_old[i-1] = C_centers_new[i-1];
            P_centers_old[i-1] = P_centers_new[i-1];
        }
        cell_to_vertex_interpolation_old();
        set_boundary_conditions_P();
        set_boundary_conditions_U();
        set_boundary_conditions_V();
        set_boundary_conditions_C();
        if(main == 1.0e+004){
            ofstream yFile50000("out10K.dat");
            yFile50000  << "VARIABLES= \"X\", \"Y\", \"P\", \"U\", \"V\", \"C\"" <<"\n";
            for(int i = 0; i<node_num; i++){
                yFile50000 << nodes[i][0] << " " << nodes[i][1] <<" "<<P_nodes[i]<<" "<<U_nodes[i]<<" "<<V_nodes[i]<<" "<<C_nodes[i]<<"\n";
            }
            yFile50000.close();
        }else if(main == 5.0e+004){
            ofstream yFile50000("out50K.dat");
            yFile50000  << "VARIABLES= \"X\", \"Y\", \"P\", \"U\", \"V\", \"C\"" <<"\n";
            for(int i = 0; i<node_num; i++){
                yFile50000 << nodes[i][0] << " " << nodes[i][1] <<" "<<P_nodes[i]<<" "<<U_nodes[i]<<" "<<V_nodes[i]<<" "<<C_nodes[i]<<"\n";
            }
            yFile50000.close();
        }else if(main == 1.0e+005){
            ofstream yFile10("out100K.dat");
            yFile10 << "VARIABLES= \"X\", \"Y\", \"P\", \"U\", \"V\", \"C\"" <<"\n";
            for(int i = 0; i<node_num; i++){
                yFile10 << nodes[i][0] << " " << nodes[i][1] <<" "<<P_nodes[i]<<" "<<U_nodes[i]<<" "<<V_nodes[i]<<" "<<C_nodes[i]<<"\n";
            }
            yFile10.close();
        }else if(main == 5.0e+005){
            ofstream yFile10("out500K.dat");
            yFile10 << "VARIABLES= \"X\", \"Y\", \"P\", \"U\", \"V\", \"C\"" <<"\n";
            for(int i = 0; i<node_num; i++){
                yFile10 << nodes[i][0] << " " << nodes[i][1] <<" "<<P_nodes[i]<<" "<<U_nodes[i]<<" "<<V_nodes[i]<<" "<<C_nodes[i]<<"\n";
            }
            yFile10.close();
        }else if(main == 1.0e+006){
            ofstream yFile10("out1M.dat");
            yFile10 << "VARIABLES= \"X\", \"Y\", \"P\", \"U\", \"V\", \"C\"" <<"\n";
            for(int i = 0; i<node_num; i++){
                yFile10 << nodes[i][0] << " " << nodes[i][1] <<" "<<P_nodes[i]<<" "<<U_nodes[i]<<" "<<V_nodes[i]<<" "<<C_nodes[i]<<"\n";
            }
            yFile10.close();
        }else if(main == 1.5e+006){
            ofstream yFile10("out1M500K.dat");
            yFile10 << "VARIABLES= \"X\", \"Y\", \"P\", \"U\", \"V\", \"C\"" <<"\n";
            for(int i = 0; i<node_num; i++){
                yFile10 << nodes[i][0] << " " << nodes[i][1] <<" "<<P_nodes[i]<<" "<<U_nodes[i]<<" "<<V_nodes[i]<<" "<<C_nodes[i]<<"\n";
            }
            yFile10.close();
        }else if(main == 2.0e+006){
            ofstream yFile100("out2M.dat");
            yFile100  << "VARIABLES= \"X\", \"Y\", \"P\", \"U\", \"V\", \"C\"" <<"\n";
            for(int i = 0; i<node_num; i++){
                yFile100 << nodes[i][0] << " " << nodes[i][1] << " " <<P_nodes[i]<<" "<<U_nodes[i]<<" "<<V_nodes[i]<<" "<<C_nodes[i]<<"\n";
            }
            yFile100.close();
        }else if(main == 2.5e+006){
            ofstream yFile500("out2M500K.dat");
            yFile500  << "VARIABLES= \"X\", \"Y\", \"P\", \"U\", \"V\", \"C\"" <<"\n";
            for(int i = 0; i<node_num; i++){
                yFile500 << nodes[i][0] << " " << nodes[i][1] <<" "<<P_nodes[i]<<" "<<U_nodes[i]<<" "<<V_nodes[i]<<" "<<C_nodes[i]<<"\n";
            }
            yFile500.close();
        }else if(main == 3.0e+006){
            ofstream yFile1000("out3M.dat");
            yFile1000 <<"VARIABLES= \"X\", \"Y\", \"P\", \"U\", \"V\", \"C\"" <<"\n";
            for(int i = 0; i<node_num; i++){
                yFile1000 << nodes[i][0] << " " << nodes[i][1] <<" "<<P_nodes[i]<<" "<<U_nodes[i]<<" "<<V_nodes[i]<<" "<<C_nodes[i]<<"\n";
            }
            yFile1000.close();
        }else if(main == 3.5e+006){
            ofstream yFile50000("out3M500K.dat");
            yFile50000  << "VARIABLES= \"X\", \"Y\", \"P\", \"U\", \"V\", \"C\"" <<"\n";
            for(int i = 0; i<node_num; i++){
                yFile50000 << nodes[i][0] << " " << nodes[i][1] <<" "<<P_nodes[i]<<" "<<U_nodes[i]<<" "<<V_nodes[i]<<" "<<C_nodes[i]<<"\n";
            }
            yFile50000.close();
        }else if(main == 4.0e+006){
            ofstream yFile50000("out4M.dat");
            yFile50000  << "VARIABLES= \"X\", \"Y\", \"P\", \"U\", \"V\", \"C\"" <<"\n";
            for(int i = 0; i<node_num; i++){
                yFile50000 << nodes[i][0] << " " << nodes[i][1] <<" "<<P_nodes[i]<<" "<<U_nodes[i]<<" "<<V_nodes[i]<<" "<<C_nodes[i]<<"\n";
            }
            yFile50000.close();
        }else if(main == 4.5e+006){
            ofstream yFile50000("out4M500K.dat");
            yFile50000  << "VARIABLES= \"X\", \"Y\", \"P\", \"U\", \"V\", \"C\"" <<"\n";
            for(int i = 0; i<node_num; i++){
                yFile50000 << nodes[i][0] << " " << nodes[i][1] <<" "<<P_nodes[i]<<" "<<U_nodes[i]<<" "<<V_nodes[i]<<" "<<C_nodes[i]<<"\n";
            }
            yFile50000.close();
        }
        // printf("  max_diff: %.12f\n", max_diff);
    }
    ofstream yFile("out5M.dat");
    yFile << "VARIABLES= \"X\", \"Y\", \"P\", \"U\", \"V\", \"C\"" <<"\n";
    for(int i = 0; i<node_num; i++){
        yFile << nodes[i][0] << " " << nodes[i][1] <<" "<<P_nodes[i]<<" "<<U_nodes[i]<<" "<<V_nodes[i]<<" "<<C_nodes[i]<<"\n";
    }
    yFile.close();
}
int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    nodes_coor();
    cout<<"1"<<"\n";
    cell_to_node();
    cout<<"2"<<"\n";
    center_coordinates_generator();
    cout<<"3"<<"\n";
    face_links();
    cout<<"4"<<"\n";
    cout<<"Bound "<<boundary_face.size()<<endl;
    vector<double> v1, v2;
    double A, V;
    for(int i=0; i<face_num; i++){
        v1 = normal(i+1);
        v2 = face_center(i+1);
        f_centers.push_back(v2);
        A = area(i+1);
        face_areas.push_back(A);
        sn.push_back(v1);
        v1.clear();
        v2.clear();
        U_faces.push_back(0);
        P_faces.push_back(0);
        V_faces.push_back(0);
        C_faces.push_back(0);
    }
    for(int i=0; i<center_num; i++){
        V = volume(i+1);
        f_volume[i] = V;
        U_centers_new[i]=0;
        U_centers_old[i]=0;
        P_centers_new[i]=0;
        P_centers_old[i]=0;
        V_centers_new[i]=0;
        V_centers_old[i]=0;
        C_centers_new[i]=0;
        C_centers_old[i]=0;
    }
    for(int i=0; i<node_num; i++){
        U_nodes[i] = 0;
        P_nodes[i] = 0;
        V_nodes[i] = 0;
        C_nodes[i] = 0;
    }
    set_boundary_conditions_U();
    cout<<"5"<<"\n";
    set_boundary_conditions_V();
    cout<<"6"<<"\n";
    set_boundary_conditions_P();
    cout<<"7"<<"\n";
    set_boundary_conditions_C();
    cout<<"8"<<"\n";
    fvm();
    cout<<"9"<<"\n";
    return 0;
}