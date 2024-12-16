#include <bits/stdc++.h>  
using namespace std;
const int node_num=800;
map<int, vector<double> > nodes;
const int center_num=741;
map<int, vector<double> > centers;
int face_num;
map<vector<int> , int> l_cell_to_face;
map<vector<int> , int> l_cell_to_node;
map<vector<int> , int> l_face_to_cell;
map<vector<int> , int> l_face_to_node;
set<int> l_node_to_nodes[node_num];
set<int> l_center_to_centers[center_num];
map<int, vector<int> > node_to_cell;
map<vector<int> , double> sn;
vector<int> boundary_face;
vector<int> nodes_boundary;
double k[center_num];
vector<double> k_boundary;
double l1_nodes[node_num];
double l1_centers[center_num];
double l2_nodes[node_num];
double l2_centers[center_num];
double u_star_nodes[node_num];
double u_star_centers[center_num];
double v_star_nodes[node_num];
double v_star_centers[center_num];
double uv_derivative_centers[center_num];
double uv_derivative_nodes[node_num];
double dP_dx_centers[center_num];
double dP_dy_centers[center_num];
double dP_dx_nodes[node_num];
double dP_dy_nodes[node_num];
double u_n_1[node_num];
double v_n_1[node_num];
double P_nodes[node_num];
double P_nodes_copy[node_num];
vector<double> P_faces;
double P_centers[center_num];
double P_centers_copy[center_num];
double f[node_num];
const double e=0.000001;
double gauss_seidel_coeff[center_num][center_num];
double gauss_seidel_b[center_num];
double coeff[node_num][node_num];
double b[node_num];
double u = 1;
double v = 0;
double d = 0.1;
double delt = 0.5;
double ro = 1;
vector<double> split_double(string line, char delimeter){
    string my_str = "";
    vector<double> v;
    for(int i; i<line.size(); i++){
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
    for(int i; i<line.size(); i++){
        if((line[i] == delimeter) or (i==line.size()-1)){
            if(line[i+1] == delimeter){
                continue;
            }
            if(i==line.size()-1){
                my_str+=line[i];  
            }
            if(my_str[0]== 'q'){
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
    ifstream NodesFile("sq_coor.txt");
    vector<double> v; 
    int cnt = 1;
    while (getline(NodesFile, myText)) {
        v = split_double(myText, ' ');
        // cnt = int(v[v.begin()]);
        v.erase(v.begin());
        v.erase(v.end()-1);
        nodes[cnt] = v;
        v.clear();
        cnt++;
    }
    NodesFile.close();
}
void cell_to_node(){
    string myText;
    ifstream NodesFile("sq_links.txt");
    vector<int> v; 
    vector<int> v1; 
    while (getline(NodesFile, myText)) {
        v = split_int(myText, ' ');
        int cell = v[0];
        v.erase(v.begin());
        v.erase(v.begin());
        for(int i=1; i<5; i++){
            v1.push_back(cell);
            v1.push_back(i);
            l_cell_to_node[v1] = v[i-1];
            if(node_to_cell.find(v[i-1]) != node_to_cell.end()){
               node_to_cell[v[i-1]].push_back(cell);
            }else{
                vector<int> temp;
                temp.push_back(cell);
                node_to_cell[v[i-1]] = temp;
                temp.clear();
            }
            v1.clear();
        }
        v.clear();
    }
    NodesFile.close();
}
void center_coordinates_generator(){//Checked without output
    vector<int> v;
    vector<double> v1;
    double x =0;
    double y =0;
    // double z =0;
    for(int i = 1; i < center_num + 1; i++){
        v.push_back(i);
        for(int j = 1; j<5; j++){
            v.push_back(j);
            register int node = l_cell_to_node[v];
            x += nodes[node][0];
            y += nodes[node][1];
            // z += nodes[node][2];

            v.erase(v.end()-1);
        }
        v1.push_back(x/double(4));
        v1.push_back(y/double(4));
        // v1.push_back(z/double(4));
        centers[i]=v1;
        v.clear();
        v1.clear();
        x = 0; 
        y = 0;
        // z = 0;
    }
}
void face_links(){
    map< set<int>, vector<int> > nodes_to_cell;
    vector<int> key;
    vector<int> key1;
    register int cnt = 1;
    vector<int> trg_nodes;
    set<int> nodes_ind;
    vector< vector<int> > c_t_f(center_num);
    for(int i=1; i<center_num+1; i++){
        key.push_back(i);
        for(int j=1; j<5; j++){
            key.push_back(j);
            register int trg_node = l_cell_to_node[key];
            trg_nodes.push_back(trg_node);
            key.erase(key.end()-1);
        }
        key.clear();
        register int m;
        for(int j=1; j<5; j++){
            if(j==4){
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
        register int s_cnt = 1;
        int node1;
        int node2;
        while(s_it != m_it->first.end()){
            key.push_back(f_cnt); 
            key.push_back(s_cnt);
            l_face_to_node[key] = *s_it;
            if(s_cnt == 1){
                node1 = *s_it;
            }else if(s_cnt == 2){
                node2 = *s_it;
            }
            s_cnt++;
            s_it++;
            key.clear();
            
        }
        l_node_to_nodes[node1-1].insert(node2);
        l_node_to_nodes[node2-1].insert(node1);
        if(node2 ==41 or node1 == 41){
            cout<<"face "<< f_cnt<<" node " << node1<< " "<<node2<<endl;

        }
        key.push_back(f_cnt);
        for(int i=1; i<key1.size()+1; i++){
            c_t_f[key1[i-1]-1].push_back(f_cnt);
            key.push_back(i);
            l_face_to_cell[key] = key1[i-1]; 
            key.erase(key.end()-1);
            if(key1.size() == 1){
                boundary_face.push_back(f_cnt);
                key.push_back(i+1);
                key.erase(key.end()-1);
                l_face_to_cell[key] = key1[i-1]; 
            }   
            // l_center_to_centers[key1[0]].insert(key1[1]);
            // l_center_to_centers[key1[1]].insert(key1[0]);
            
        }
        f_cnt++;
        m_it++;
        key1.clear();
        key.clear();
    }
    key.push_back(boundary_face[1]);
    key.push_back(1);
    key1.push_back(boundary_face[2]);
    key1.push_back(0);
    if(l_face_to_node[key] != l_face_to_node[key1]){
        int temp = boundary_face[0];
        boundary_face[0] = boundary_face[1];
        boundary_face[1] = temp;
    }
    key.clear();
    key1.clear();
    for(int i=1; i<center_num+1; i++){
        key.push_back(i);
        for(int j=1; j<c_t_f[i-1].size()+1; j++){
            key.push_back(j);
            l_cell_to_face[key] = c_t_f[i-1][j-1];
            key.erase(key.end()-1);
        }
        key.clear();
    }
}
bool is_boundary_face(int face_ind){
    vector<int> key1;
    key1.push_back(face_ind);
    key1.push_back(1);
    vector<int> key2; 
    key2.push_back(face_ind);
    key2.push_back(2);
    if(l_face_to_cell[key1] == l_face_to_cell[key2]){
        return true;
    }
    return false;
}
vector<double> face_center(int face_ind){
    vector <int> key; 
    key.push_back(face_ind); 
    key.push_back(1);
    vector<double> n1 = nodes[l_face_to_node[key]];
    key.erase(key.end()-1);
    key.push_back(2);
    vector<double> n2 = nodes[l_face_to_node[key]];
    vector<double> ans; 
    ans.push_back((n1[0] + n2[0])/2);
    ans.push_back((n1[1] + n2[1])/2);
    return ans;
}
double find_max(double v[center_num]){
    double max = v[1];
    for(int i=1; i<center_num; i++){
        if(v[i]>max){
            max = v[i];
        }
    }
    return max;
}
void jacobi(){
    double to_check = 1;
    double deltx = 0.0128205;
    for(int j=0; j<100000; j++){
        // to_check = find_max(P_nodes_copy); 
        // cout << "j " << j << endl;
        double cnt = 0;
        int node_cnt = 0;
        for(int i=0; i<node_num; i++){
            if(l_node_to_nodes[i].size() != 4){
                // cout<<"node "<<i<<endl;
                node_cnt++;
                continue;
            }
            set <int>::iterator s_it = l_node_to_nodes[i].begin();
            while(s_it != l_node_to_nodes[i].end()){
                cnt += P_nodes_copy[*(s_it)-1];
                s_it++;
            }
            P_nodes[i] = 0.25 * (cnt + f[i] * deltx * deltx);
            cnt = 0;
        }
        // cout<<"b_node "<<node_cnt<<endl;
        for(int i=0; i<node_num; i++){
            P_nodes_copy[i] = P_nodes[i];
        }
    }
}
void set_boundary_conditions(){
    for(int i=0; i<face_num; i++){
        P_faces.push_back(0);
    }
    for(int i=0; i<node_num; i++){
        P_nodes[i] = 0;
        f[i] = 0;
    }
    vector<int> v; 
    for(int i=0; i<boundary_face.size(); i++){
        int b_f = boundary_face[i];
        v.push_back(b_f); 
        v.push_back(1);
        int n1 = l_face_to_node[v];
        double x1 = nodes[n1][0];
        double y1 = nodes[n1][1];
        v.erase(v.end()-1);
        v.push_back(2);
        int n2 = l_face_to_node[v];
        double x2 = nodes[n2][0];
        double y2 = nodes[n2][1];
        P_nodes[n1-1] = (sin(x1+2*y1))+(exp(2*x1+3*y1));
        nodes_boundary.push_back(n1);
        P_nodes[n2-1] = (sin(x2+2*y2))+(exp(2*x2+3*y2));
        nodes_boundary.push_back(n2);
        P_faces[b_f-1] = (P_nodes[n1-1] + P_nodes[n2-1])/2;
        v.clear();
    }
    for(int i=0; i<center_num; i++){
        P_centers[i]=0;
        P_centers_copy[i]=0;
    } 
}
// void jacobi1(){
//     double to_check = 1;
//     // double deltx = 0.44444;
//     double deltx = 0.183673;

//         // int cnt1 =0;

//     for(int j=0; j<300; j++){
//         // to_check = find_max(P_nodes_copy); 
//         // vector <int> node_inds;
//         // cout << "j " << j << endl;
//         double cnt = 0;
//         for(int i=0; i<node_num; i++){
//             if(l_node_to_nodes[i].size() != 4){
//                 // cnt1++;
//                 continue;
//             }
//             set <int>::iterator s_it = l_node_to_nodes[i].begin();
//             while(s_it != l_node_to_nodes[i].end()){
//                 cnt += u_nodes_copy[*s_it-1];
//                 // node_inds.push_back(*s_it);
//                 //cout << "node " << *s_it << " u " << u_nodes_copy[*s_it-1] << endl;
//                 s_it++;
//             }
//             // cout << "cnt " << cnt << endl;
//             // cout << "coor " << nodes[i+1][0] <<" "<<nodes[i+1][1] << endl;

//             u_nodes[i] = 0.25 *(cnt + f[i] * deltx * deltx);
//             //if(nodesnode_inds[0])
//             l1_nodes[i] = u_nodes[i] - 1 * (u_nodes_copy[i+1] - u_nodes_copy[i]);
//             // f[i] = - (cnt - 4 * P_nodes[i]) / (deltx * deltx);            
//             cnt = 0;
//             // cout << "u " << u_nodes[i] << endl;
//         }
//     // cout<<"CNT "<<cnt1<<endl;

//         for(int i=0; i<node_num; i++){
//             u_nodes_copy[i] = u_nodes[i];

//         }
//         // for(int i=0; i<node_num; i++){
//         //     cout << "oldP " << P_nodes_copy[i] << endl;
//         //     cout << "newP " << P_nodes[i] << endl;

//         // }
//     }
// }
void gauss_seidel_nodes(){
    double to_check = 1;
    while(abs(to_check - find_max(P_centers))>e){
        to_check = find_max(P_centers);
        for(int i=0; i<center_num; i++){
            P_nodes_copy[i] = b[i];
            for(int j=0; j<center_num; j++){
                if (j == i){
                    continue;
                }
                P_nodes_copy[i] = P_nodes_copy[i] - (coeff[i][j] * P_nodes[j]);
                P_nodes[i] = (P_nodes_copy[i]/coeff[i][i]);
            }
        }
    }
}
void gauss_seidel(){
    double to_check = 1;
    while(abs(to_check - find_max(P_centers))>e){
        to_check = find_max(P_centers);
        for(int i=0; i<center_num; i++){
            P_centers_copy[i] = gauss_seidel_b[i];
            for(int j=0; j<center_num; j++){
                if (j == i){
                    continue;
                }
                P_centers_copy[i] = P_centers_copy[i] - (gauss_seidel_coeff[i][j] * P_centers[j]);
                P_centers[i] = (P_centers_copy[i]/gauss_seidel_coeff[i][i]);
            }
        }
    }
}
vector<double> normal(int face_ind){
    vector <double> ans; 
    vector <int> key; 
    key.push_back(face_ind); 
    key.push_back(1);
    vector<double> n1 = nodes[l_face_to_node[key]];
    key.erase(key.end()-1);
    key.push_back(2);
    vector<double> n2 = nodes[l_face_to_node[key]];
    ans.push_back(n1[1] - n2[1]); 
    ans.push_back(n2[0] - n1[0]);  
    double len = pow(pow(ans[0], 2) + pow(ans[1], 2), 0.5); 
    for(int i=0; i<ans.size(); i++){
        ans[i] /= len;
    }
    return ans; 
}
double area(int face_ind){
    vector <int> key; 
    key.push_back(face_ind); 
    key.push_back(1);
    vector<double> n1 = nodes[l_face_to_node[key]];
    key.erase(key.end()-1);
    key.push_back(2);
    vector<double> n2 = nodes[l_face_to_node[key]];
    return pow((pow((n1[0] - n2[0]), 2) + pow((n1[1] - n2[1]), 2)) , 0.5);
}
double cell_to_vertex_interpolation(int node_ind){ 
    vector<double> node_coor = nodes[node_ind]; 
    vector<double> center_coor;  
    int cell_ind; 
    double nom = 0; 
    double denom = 0; 
    for(int i=0; i<node_to_cell[node_ind].size(); i++){ 
        cell_ind = node_to_cell[node_ind][i]; 
        center_coor = centers[cell_ind]; 
        double d = pow(pow((center_coor[0] - node_coor[0]), 2) + pow((center_coor[1] - node_coor[1]), 2), 0.5); 
        double P = P_centers[cell_ind - 1]; 
        nom += P/d; 
        denom += 1/d; 
    } 
    return nom/denom; 
}
void fdm_poisson(){
    // double deltx = 0.183673;
    double deltx = 0.444444;
    // double deltx;
    for(int i=0; i<node_num; i++){
        // if(l_node_to_nodes[i].size() != 4){
        //     continue;
        // }
        set <int>::iterator s_it = l_node_to_nodes[i].begin();
        // cout<<"Here"<<endl;
        b[i] = 0;
        while(s_it != l_node_to_nodes[i].end()){
            // deltx = fabs(nodes[i+1][0] - nodes[*s_it][0]);
            // cout<<"nodes "<<i+1<<" "<<*s_it<<endl;
            if(l_node_to_nodes[i].size() == 4){
                // cout<<"here"<<endl;
                coeff[i][*(s_it)-1] = 1/pow(deltx, 2);
            }else{
                b[i] += P_nodes[*(s_it) - 1];
                // coeff[i][*(s_it)-1] = 0;
            }
            s_it++;
        }
        // cout<<deltx<<endl;
        coeff[i][i] = -(4/(pow(deltx, 2)));
        b[i] += P_nodes[i];
    }
    gauss_seidel_nodes();
}
void fvm_convection(){
    for(int i=1; i<center_num+1; i++){
        vector<int> key;
        key.push_back(i);
        double ac=0;
        double bc=0;
        vector<double> center_coor = centers[i];
        for(int j=1; j<5; j++){
            key.push_back(j);
            register int face_ind = l_cell_to_face[key];
            vector<int> key1;
            key1.push_back(face_ind);
            double deltx;
            double delty;
            double nx; 
            double ny;
            double valid_denom;
            if(find(boundary_face.begin(), boundary_face.end(), face_ind) != boundary_face.end()){   
                vector<double> face_coor = face_center(face_ind);
                deltx = face_coor[0] - center_coor[0];
                delty = face_coor[1] - center_coor[1];
                key1.push_back(1);
                nx = sn[key1];
                key1.erase(key1.end()-1);
                key1.push_back(2);
                ny = sn[key1];
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
                // cout<<" nx "<< nx<< " ny "<< ny<< " deltx "<< deltx<< " delty "<<delty<<" valid denom "<< valid_denom<<endl;
                bc += (area(face_ind))*P_faces[face_ind-1]*((d*valid_denom) - u*nx - v*ny);
                ac += (area(face_ind))*((d*valid_denom) - u*nx - v*ny);
                key1.clear();
            }else{
                key1.push_back(1);
                nx = sn[key1];
                register int out_center_ind = l_face_to_cell[key1]; 
                key1.erase(key1.end()-1);
                key1.push_back(2);
                vector<double> c2 = centers[out_center_ind];
                ny = sn[key1];
                if(out_center_ind == i){
                    out_center_ind = l_face_to_cell[key1];
                    c2 = centers[out_center_ind];
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
                // cout<<" nx "<< nx<< " ny "<< ny<< " deltx "<< deltx<< " delty "<<delty<<" valid denom "<< valid_denom<<endl;
                gauss_seidel_coeff[i-1][out_center_ind-1] = (-1)*(area(face_ind)*((d*valid_denom) - u*nx - v*ny));
                ac += (area(face_ind))*((d*valid_denom) - u*nx - v*ny);
                key1.clear();
            }
            key.erase(key.end()-1);
        }
        gauss_seidel_coeff[i-1][i-1] = ac;
        if(ac = 0){
            cout<<"YES"<<endl;
        }
        gauss_seidel_b[i-1] = bc;
        ac = 0;
        bc = 0; 
        key.clear();
    }
    gauss_seidel();
}
void fvm_poisson(){
    for(int i=1; i<center_num+1; i++){
        vector<int> key;
        key.push_back(i);
        double ac=0;
        double bc=0;
        vector<double> center_coor = centers[i];
        for(int j=1; j<5; j++){
            key.push_back(j);
            register int face_ind = l_cell_to_face[key];
            vector<int> key1;
            key1.push_back(face_ind);
            double deltx;
            double delty;
            double nx; 
            double ny;
            double valid_denom;
            if(find(boundary_face.begin(), boundary_face.end(), face_ind) != boundary_face.end()){   
                vector<double> face_coor = face_center(face_ind);
                deltx = face_coor[0] - center_coor[0];
                delty = face_coor[1] - center_coor[1];
                key1.push_back(1);
                nx = sn[key1];
                key1.erase(key1.end()-1);
                key1.push_back(2);
                ny = sn[key1];
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
                
                bc += (area(face_ind))*P_faces[face_ind-1]*valid_denom;
                ac += (area(face_ind))*valid_denom;
                key1.clear();
            }else{
                key1.push_back(1);
                nx = sn[key1];
                register int out_center_ind = l_face_to_cell[key1]; 
                key1.erase(key1.end()-1);
                key1.push_back(2);
                vector<double> c2 = centers[out_center_ind];
                ny = sn[key1];
                if(out_center_ind == i){
                    out_center_ind = l_face_to_cell[key1];
                    c2 = centers[out_center_ind];
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
                gauss_seidel_coeff[i-1][out_center_ind-1] = (-1)*(area(face_ind)*valid_denom);
                ac += (area(face_ind))*valid_denom;
                key1.clear();
            }
            key.erase(key.end()-1);
        }
        gauss_seidel_coeff[i-1][i-1] = ac;
        gauss_seidel_b[i-1] = bc;
        ac = 0;
        bc = 0; 
        key.clear();
    }
    gauss_seidel();
}
void set_boundary_conditions_u(){
    for(int i=0; i<face_num; i++){
        P_faces.push_back(0);
    }
    for(int i=0; i<node_num; i++){
        P_nodes[i] = 0;
        P_nodes_copy[i] = 0;
        f[i] = 0;
    }
    vector<int> v; 
    for(int i=0; i<boundary_face.size(); i++){
        int b_f = boundary_face[i];
        v.push_back(b_f); 
        v.push_back(1);
        int n1 = l_face_to_node[v];
        double x1 = nodes[l_face_to_node[v]][0];
        double y1 = nodes[l_face_to_node[v]][1]; 
        v.erase(v.end()-1);
        v.push_back(2);
        int n2 = l_face_to_node[v];
        double x2 = nodes[l_face_to_node[v]][0];
        double y2 = nodes[l_face_to_node[v]][1]; 
        if((fabs(x1-1)==0) and (fabs(x2-1)==0) and (y1>3) and (y1<4) and (y2>3) and (y2<4)){
        // if((fabs(x1-1)==0) and (fabs(x2-1)==0)){

            nodes_boundary.push_back(n1);
            nodes_boundary.push_back(n2);
            P_nodes[n1-1] = 1;
            P_nodes_copy[n1-1] = 1;
            P_nodes[n2-1] = 1;
            P_nodes_copy[n2-1] = 1;
            P_faces[b_f-1] = 1;
        }
        v.clear();
    }
    for(int i=0; i<center_num; i++){
        P_centers[i]=0;
        P_centers_copy[i]=0;
    } 
}
void set_boundary_conditions_v(){
    nodes_boundary.clear();
    for(int i=0; i<face_num; i++){
        P_faces[i] = 0;
    }
    for(int i=0; i<node_num; i++){
        P_nodes[i] = 0;
        P_nodes_copy[i] = 0;
    }
    vector<int> v; 
    for(int i=0; i<boundary_face.size(); i++){
        int b_f = boundary_face[i];
        v.push_back(b_f); 
        v.push_back(1);
        int n1 = l_face_to_node[v];
        double x1 = nodes[l_face_to_node[v]][0];
        double y1 = nodes[l_face_to_node[v]][1]; 
        v.erase(v.end()-1);
        v.push_back(2);
        int n2 = l_face_to_node[v];
        double x2 = nodes[l_face_to_node[v]][0];
        double y2 = nodes[l_face_to_node[v]][1]; 
        if((fabs(x1-1)<=0) and (fabs(x2-1)<=0) and (y1>3) and (y1<4) and (y2>3) and (y2<4)){
        // if((fabs(x1-1)<=0) and (fabs(x2-1)<=0)){
            nodes_boundary.push_back(n1);
            nodes_boundary.push_back(n2);
            P_nodes[n1-1] = 1;
            P_nodes_copy[n1-1] = 1;
            P_nodes[n2-1] = 1;
            P_nodes_copy[n2-1] = 1;
            P_faces[b_f-1] = 1;
        }
        v.clear();
    }
    for(int i=0; i<center_num; i++){
        P_centers[i]=0;
        P_centers_copy[i]=0;
    } 
}
void set_boundary_conditions_P(){
    nodes_boundary.clear();
    for(int i=0; i<face_num; i++){
        P_faces[i] = 0;
    }
    for(int i=0; i<node_num; i++){
        P_nodes[i] = 0;
        P_nodes_copy[i] = 0;
    }
    vector<int> v; 
    for(int i=0; i<boundary_face.size(); i++){

        int b_f = boundary_face[i];
        v.push_back(b_f); 
        v.push_back(1);
        int n1 = l_face_to_node[v];
        double x1 = nodes[l_face_to_node[v]][0];
        double y1 = nodes[l_face_to_node[v]][1]; 
        v.erase(v.end()-1);
        v.push_back(2);
        int n2 = l_face_to_node[v];
        double x2 = nodes[l_face_to_node[v]][0];
        double y2 = nodes[l_face_to_node[v]][1]; 
        if((fabs(x1-1)<=0) and (fabs(x2-1)<=0) and (y1>3) and (y1<4) and (y2>3) and (y2<4)){
        // if((fabs(x1-1)<=0) and (fabs(x2-1)<=0)){
            nodes_boundary.push_back(n1);
            nodes_boundary.push_back(n2);
            P_nodes[n1-1] = 1;
            P_nodes[n2-1] = 1;
            P_nodes_copy[n1-1] = 1;
            P_nodes_copy[n2-1] = 1;
            P_faces[b_f-1] = 1;
        }
        v.clear();
    }

    for(int i=0; i<center_num; i++){
        P_centers[i]=0;
        P_centers_copy[i]=0;
    } 
}
void u_star(){
    for(int i =0; i<node_num; i++){
        u_star_nodes[i] = (delt * l1_nodes[i]) + P_nodes[i];
    }
    for(int i =0; i<center_num; i++){
        u_star_centers[i] = delt * l1_centers[i];
    }
}
void v_star(){
    for(int i =0; i<node_num; i++){
        v_star_nodes[i] = (delt * l2_nodes[i]);
    }
    for(int i =0; i<center_num; i++){
        v_star_centers[i] = delt * l2_centers[i];
    }
}
// void fvm_1st_derivatives(){
//     // for(int i=0; i<center_num; i++){
//     //     // set <int>::iterator s_it = l_center_to_centers[0]->first.begin();
//     //     // while(s_it != m_it->first.end()){
//     //     //     s_it++;
//     //     // }
//     // }
//     for(int i=1; i<center_num+1; i++){
//         uv_derivative_centers[i-1] = 0;
//         vector<int> key;
//         key.push_back(i);
//         vector<double> center_coor = centers[i];
//         double denom = 0;
//         for(int j=1; j<5; j++){
//             key.push_back(j);
//             register int face_ind = l_cell_to_face[key];
//             vector<int> key1;
//             key1.push_back(face_ind);
//             double deltx;
//             double delty;
//             double nx; 
//             double ny;
//             if(find(boundary_face.begin(), boundary_face.end(), face_ind) != boundary_face.end()){   
//                 vector<double> face_coor = face_center(face_ind);
//                 deltx = face_coor[0] - center_coor[0];
//                 delty = face_coor[1] - center_coor[1];
//                 key1.push_back(1);
//                 nx = sn[key1];
//                 key1.erase(key1.end()-1);
//                 key1.push_back(2);
//                 ny = sn[key1];
//                 if(((deltx>0) and (nx<0)) or ((deltx<0) and (nx>0))){
//                     nx*=(-1);
//                 }
//                 if(((delty>0) and (ny<0)) or ((delty<0) and (ny>0))){
//                     ny*=(-1);
//                 }
//                 uv_derivative_centers[i-1] += (area(face_ind)) * P_faces[face_ind-1]*(nx+ny);
//                 key1.clear();
//             }else{
//                 key1.push_back(1);
//                 nx = sn[key1];
//                 register int out_center_ind = l_face_to_cell[key1]; 
//                 key1.erase(key1.end()-1);
//                 key1.push_back(2);
//                 vector<double> c2 = centers[out_center_ind];
//                 ny = sn[key1];
//                 if(out_center_ind == i){
//                     out_center_ind = l_face_to_cell[key1];
//                     c2 = centers[out_center_ind];
//                 }
//                 deltx = c2[0] - center_coor[0];
//                 delty = c2[1] - center_coor[1];
//                 if(((deltx>0) and (nx<0)) or ((deltx<0) and (nx>0))){
//                     nx*=(-1);
//                 }
//                 if(((delty>0) and (ny<0)) or ((delty<0) and (ny>0))){
//                     ny*=(-1);
//                 }
//                 denom += 
//                 uv_derivative_centers[i-1] -= area(face_ind) * ((u_star_centers[out_center_ind-1] * nx) + v_star_centers[out_center_ind-1] * ny);
//                 key1.clear();
//             }
//             key.erase(key.end()-1);
//         }
//         uv_derivative_centers[i-1] /= denom;
//         denom = 0;
//         key.clear();
//     }
//     for(int i=0; i<center_num; i++){
//         P_centers[i] = uv_derivative_centers[i];
//     }
//     for(int i=1; i<node_num+1; i++){
//         if(find(nodes_boundary.begin(), nodes_boundary.end(), i) != nodes_boundary.end()){
//             // cout<<i<<endl;
//             continue;
//         }else{
//             uv_derivative_nodes[i-1] = cell_to_vertex_interpolation(i);
//         }
//     }
//     // gauss_seidel();
// }
void fvm_1st_derivatives(){
    for(int i=1; i<center_num+1; i++){
        vector<int> key;
        key.push_back(i);
        double denom_u = 0;
        double denom_v = 0;
        vector<double> center_coor = centers[i];
        u_star_centers[i-1] = 0;
        v_star_centers[i-1] = 0;
        for(int j=1; j<5; j++){
            key.push_back(j);
            register int face_ind = l_cell_to_face[key];
            vector<int> key1;
            key1.push_back(face_ind);
            double deltx;
            double delty;
            double nx; 
            double ny;
            if(find(boundary_face.begin(), boundary_face.end(), face_ind) != boundary_face.end()){   
                vector<double> face_coor = face_center(face_ind);
                deltx = face_coor[0] - center_coor[0];
                delty = face_coor[1] - center_coor[1];
                key1.push_back(1);
                nx = sn[key1];
                key1.erase(key1.end()-1);
                key1.push_back(2);
                ny = sn[key1];
                if(((deltx>0) and (nx<0)) or ((deltx<0) and (nx>0))){
                    nx*=(-1);
                }
                if(((delty>0) and (ny<0)) or ((delty<0) and (ny>0))){
                    ny*=(-1);
                }
                denom_u += (area(face_ind) * nx);
                denom_v += (area(face_ind) * ny);
                // if(i<5 and i>0){
                //     cout<<i<<" AREa "<<area(face_ind)<<" nx "<<nx<<" ny "<<ny<<endl;
                //     cout<<"denom "<<denom_u<<" "<<denom_v<<endl;
                // }
                u_star_centers[i-1] -= (area(face_ind))*P_faces[face_ind-1] * nx;
                v_star_centers[i-1] -= (area(face_ind))*P_faces[face_ind-1] * ny;
                key1.clear();
            }else{
                key1.push_back(1);
                nx = sn[key1];
                register int out_center_ind = l_face_to_cell[key1]; 
                key1.erase(key1.end()-1);
                key1.push_back(2);
                vector<double> c2 = centers[out_center_ind];
                ny = sn[key1];
                if(out_center_ind == i){
                    out_center_ind = l_face_to_cell[key1];
                    c2 = centers[out_center_ind];
                }
                deltx = c2[0] - center_coor[0];
                delty = c2[1] - center_coor[1];
                if(((deltx>0) and (nx<0)) or ((deltx<0) and (nx>0))){
                    nx*=(-1);
                }
                if(((delty>0) and (ny<0)) or ((delty<0) and (ny>0))){
                    ny*=(-1);
                }
                denom_u += (area(face_ind) * nx);
                denom_v += (area(face_ind) * ny);
                // if(i<5 and i>0){
                //     cout<<i<<" AREa "<<area(face_ind)<<" nx "<<nx<<" ny "<<ny<<endl;
                //     cout<<"denom "<<denom_u<<" "<<denom_v<<endl;
                // }
                u_star_centers[i-1] += area(face_ind) * u_star_centers[out_center_ind-1] * nx;
                v_star_centers[i-1] += area(face_ind) * v_star_centers[out_center_ind-1] * ny;
                key1.clear();
            }
            key.erase(key.end()-1);   
        }

        uv_derivative_centers[i-1] = 0;
        if(!(isnan(1/denom_u))){
            // u_star_centers[i-1] /=denom_u;
            uv_derivative_centers[i-1] += u_star_centers[i-1];
        }
        if(!(isnan(1/denom_v))){
            // v_star_centers[i-1] /=denom_v;  
            uv_derivative_centers[i-1] += v_star_centers[i-1];
        }
        
        // dP_dx_centers[i-1] = u_star_centers[i-1];
        // dP_dy_centers[i-1] = v_star_centers[i-1];
        // denom_u = 0;
        // denom_v = 0;
        key.clear();
    }
    for(int i=0; i<center_num; i++){
        P_centers[i] = uv_derivative_centers[i];
    }
    for(int i=1; i<node_num+1; i++){
        if(find(nodes_boundary.begin(), nodes_boundary.end(), i) != nodes_boundary.end()){
            continue;
        }else{
            uv_derivative_nodes[i-1] = (ro/delt) * cell_to_vertex_interpolation(i);
        }
    }
    //  for(int i=0; i<center_num; i++){
    //     P_centers[i] = dP_dy_centers[i];
    // }
    // for(int i=1; i<node_num+1; i++){
    //     if(find(nodes_boundary.begin(), nodes_boundary.end(), i) != nodes_boundary.end()){
    //         continue;
    //     }else{
    //         dP_dy_nodes[i-1] = (ro/delt) * cell_to_vertex_interpolation(i);
    //     }
    // }
    ofstream UyFile("uv_derivative.dat");
    UyFile << "VARIABLES= \"X\", \"Y\", \"L\"" <<endl;
    for(int i=0; i<node_num; i++){
        UyFile << nodes[i+1][0] << " " << nodes[i+1][1] <<" "<<uv_derivative_nodes[i] <<endl;
    }
    UyFile.close();
    // gauss_seidel();
}
void fvm_2nd_derivatives(){
    for(int i=1; i<center_num+1; i++){
        vector<int> key;
        key.push_back(i);
        double denom_u = 0;
        double denom_v = 0;
        vector<double> center_coor = centers[i];
        u_star_centers[i-1] = 0;
        v_star_centers[i-1] = 0;
        for(int j=1; j<5; j++){
            key.push_back(j);
            register int face_ind = l_cell_to_face[key];
            vector<int> key1;
            key1.push_back(face_ind);
            double deltx;
            double delty;
            double nx; 
            double ny;
            double valid_denom;
            if(find(boundary_face.begin(), boundary_face.end(), face_ind) != boundary_face.end()){   
                vector<double> face_coor = face_center(face_ind);
                deltx = face_coor[0] - center_coor[0];
                delty = face_coor[1] - center_coor[1];
                key1.push_back(1);
                nx = sn[key1];
                key1.erase(key1.end()-1);
                key1.push_back(2);
                ny = sn[key1];
                if(((deltx>0) and (nx<0)) or ((deltx<0) and (nx>0))){
                    nx*=(-1);
                }
                if(((delty>0) and (ny<0)) or ((delty<0) and (ny>0))){
                    ny*=(-1);
                }
                valid_denom=0;
                if(deltx != 0){
                    valid_denom += (nx/deltx);
                    denom_u += (area(face_ind) * (nx/deltx));
                }
                if(delty != 0){
                    valid_denom += (ny/delty);
                    denom_v += (area(face_ind) * (ny/delty));
                }
                if(i<5 and i>0){
                    cout<<i<<" AREa "<<area(face_ind)<<" nx "<<nx<<" ny "<<ny<<endl;
                    cout<<"denom "<<denom_u<<" "<<denom_v<<endl;
                }
                u_star_centers[i-1] += (area(face_ind))*P_faces[face_ind-1] * valid_denom;
                v_star_centers[i-1] += (area(face_ind))*P_faces[face_ind-1] * valid_denom;
                key1.clear();
            }else{
                key1.push_back(1);
                nx = sn[key1];
                register int out_center_ind = l_face_to_cell[key1]; 
                key1.erase(key1.end()-1);
                key1.push_back(2);
                vector<double> c2 = centers[out_center_ind];
                ny = sn[key1];
                if(out_center_ind == i){
                    out_center_ind = l_face_to_cell[key1];
                    c2 = centers[out_center_ind];
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
                    denom_u += (area(face_ind) * (nx/deltx));
                }
                if(delty != 0){
                    valid_denom += (ny/delty);
                    denom_v += (area(face_ind) * (ny/delty));
                }
                if(i<5 and i>0){
                    cout<<i<<" AREa "<<area(face_ind)<<" nx "<<nx<<" ny "<<ny<<endl;
                    cout<<"denom "<<denom_u<<" "<<denom_v<<endl;
                }
                u_star_centers[i-1] += area(face_ind) * u_star_centers[out_center_ind-1] * valid_denom;
                v_star_centers[i-1] += area(face_ind) * v_star_centers[out_center_ind-1] * valid_denom;
                key1.clear();
            }
            key.erase(key.end()-1);   
        }
        uv_derivative_centers[i-1] = (u_star_centers[i-1]/denom_u) + (v_star_centers[i-1]/denom_v);
        // dP_dx_centers[i-1] = u_star_centers[i-1];
        // dP_dy_centers[i-1] = v_star_centers[i-1];
        // denom_u = 0;
        // denom_v = 0;
        key.clear();
    }
    for(int i=0; i<center_num; i++){
        P_centers[i] = uv_derivative_centers[i];
    }
    for(int i=1; i<node_num+1; i++){
        if(find(nodes_boundary.begin(), nodes_boundary.end(), i) != nodes_boundary.end()){
            continue;
        }else{
            uv_derivative_nodes[i-1] = (ro/delt) * cell_to_vertex_interpolation(i);
        }
    }
    //  for(int i=0; i<center_num; i++){
    //     P_centers[i] = dP_dy_centers[i];
    // }
    // for(int i=1; i<node_num+1; i++){
    //     if(find(nodes_boundary.begin(), nodes_boundary.end(), i) != nodes_boundary.end()){
    //         continue;
    //     }else{
    //         dP_dy_nodes[i-1] = (ro/delt) * cell_to_vertex_interpolation(i);
    //     }
    // }
    ofstream UyFile("uv_derivative.dat");
    UyFile << "VARIABLES= \"X\", \"Y\", \"P\"" <<endl;
    for(int i=0; i<node_num; i++){
        UyFile << nodes[i+1][0] << " " << nodes[i+1][1] <<" "<<uv_derivative_nodes[i] <<endl;
    }
    UyFile.close();
    // gauss_seidel();
}
// void find_P(){
//     for(int i=0; i<node_num; i++){
//         cout<<P_nodes[i]/(uv_derivative_nodes[i])<<endl;
//     }
// }
void fvm_dP_dx_dy(){
    for(int i=0; i<center_num; i++){
        P_centers_copy[i] = P_centers[i];
    }
    for(int i=1; i<center_num+1; i++){
        vector<int> key;
        key.push_back(i);
        double denom_x = 0;
        double denom_y = 0;
        vector<double> center_coor = centers[i];
        P_centers[i-1]=0;
        for(int j=1; j<5; j++){
            key.push_back(j);
            register int face_ind = l_cell_to_face[key];
            vector<int> key1;
            key1.push_back(face_ind);
            double deltx;
            double delty;
            double nx; 
            double ny;
            if(find(boundary_face.begin(), boundary_face.end(), face_ind) != boundary_face.end()){   
                vector<double> face_coor = face_center(face_ind);
                deltx = face_coor[0] - center_coor[0];
                delty = face_coor[1] - center_coor[1];
                key1.push_back(1);
                nx = sn[key1];
                key1.erase(key1.end()-1);
                key1.push_back(2);
                ny = sn[key1];
                if(((deltx>0) and (nx<0)) or ((deltx<0) and (nx>0))){
                    nx*=(-1);
                }
                if(((delty>0) and (ny<0)) or ((delty<0) and (ny>0))){
                    ny*=(-1);
                }
                denom_x += (area(face_ind) * nx);
                denom_y += (area(face_ind) * ny);
                // if(i<5 and i>0){
                //     cout<<i<<" AREa "<<area(face_ind)<<" nx "<<nx<<" ny "<<ny<<endl;
                //     cout<<"denom "<<denom_u<<" "<<denom_v<<endl;
                // }
                P_centers[i-1] -= (area(face_ind))*P_faces[face_ind-1] * nx;
                // Y_centers[i-1] -= (area(face_ind))*P_faces[face_ind-1] * ny;
                key1.clear();
            }else{
                key1.push_back(1);
                nx = sn[key1];
                register int out_center_ind = l_face_to_cell[key1]; 
                key1.erase(key1.end()-1);
                key1.push_back(2);
                vector<double> c2 = centers[out_center_ind];
                ny = sn[key1];
                if(out_center_ind == i){
                    out_center_ind = l_face_to_cell[key1];
                    c2 = centers[out_center_ind];
                }
                deltx = c2[0] - center_coor[0];
                delty = c2[1] - center_coor[1];
                if(((deltx>0) and (nx<0)) or ((deltx<0) and (nx>0))){
                    nx*=(-1);
                }
                if(((delty>0) and (ny<0)) or ((delty<0) and (ny>0))){
                    ny*=(-1);
                }
                denom_x += (area(face_ind) * nx);
                denom_y += (area(face_ind) * ny);
                // if(i<5 and i>0){
                //     cout<<i<<" AREa "<<area(face_ind)<<" nx "<<nx<<" ny "<<ny<<endl;
                //     cout<<"denom "<<denom_u<<" "<<denom_v<<endl;
                // }
                P_centers[i-1] += area(face_ind) * P_centers[out_center_ind-1] * nx;
                // P_centers[i-1] += area(face_ind) * P_centers[out_center_ind-1] * ny;
                key1.clear();
            }
            key.erase(key.end()-1);   
        }
        if(denom_x !=0){
            P_centers[i-1]+= (P_centers[i-1]/denom_x);
        }
        key.clear();
    }
    for(int i=1; i<node_num+1; i++){
        if(find(nodes_boundary.begin(), nodes_boundary.end(), i) != nodes_boundary.end()){
            continue;
        }else{
            dP_dx_nodes[i-1] = (ro/delt) * cell_to_vertex_interpolation(i);
        }
    }
        for(int i=1; i<center_num+1; i++){
        vector<int> key;
        key.push_back(i);
        double denom_x = 0;
        double denom_y = 0;
        vector<double> center_coor = centers[i];
        P_centers_copy[i-1] = 0;
        for(int j=1; j<5; j++){
            key.push_back(j);
            register int face_ind = l_cell_to_face[key];
            vector<int> key1;
            key1.push_back(face_ind);
            double deltx;
            double delty;
            double nx; 
            double ny;
            if(find(boundary_face.begin(), boundary_face.end(), face_ind) != boundary_face.end()){   
                vector<double> face_coor = face_center(face_ind);
                deltx = face_coor[0] - center_coor[0];
                delty = face_coor[1] - center_coor[1];
                key1.push_back(1);
                nx = sn[key1];
                key1.erase(key1.end()-1);
                key1.push_back(2);
                ny = sn[key1];
                if(((deltx>0) and (nx<0)) or ((deltx<0) and (nx>0))){
                    nx*=(-1);
                }
                if(((delty>0) and (ny<0)) or ((delty<0) and (ny>0))){
                    ny*=(-1);
                }
                denom_x += (area(face_ind) * nx);
                denom_y += (area(face_ind) * ny);
                // if(i<5 and i>0){
                //     cout<<i<<" AREa "<<area(face_ind)<<" nx "<<nx<<" ny "<<ny<<endl;
                //     cout<<"denom "<<denom_u<<" "<<denom_v<<endl;
                // }
                P_centers_copy[i-1] -= (area(face_ind))*P_faces[face_ind-1] * nx;
                // Y_centers[i-1] -= (area(face_ind))*P_faces[face_ind-1] * ny;
                key1.clear();
            }else{
                key1.push_back(1);
                nx = sn[key1];
                register int out_center_ind = l_face_to_cell[key1]; 
                key1.erase(key1.end()-1);
                key1.push_back(2);
                vector<double> c2 = centers[out_center_ind];
                ny = sn[key1];
                if(out_center_ind == i){
                    out_center_ind = l_face_to_cell[key1];
                    c2 = centers[out_center_ind];
                }
                deltx = c2[0] - center_coor[0];
                delty = c2[1] - center_coor[1];
                if(((deltx>0) and (nx<0)) or ((deltx<0) and (nx>0))){
                    nx*=(-1);
                }
                if(((delty>0) and (ny<0)) or ((delty<0) and (ny>0))){
                    ny*=(-1);
                }
                denom_x += (area(face_ind) * nx);
                denom_y += (area(face_ind) * ny);
                // if(i<5 and i>0){
                //     cout<<i<<" AREa "<<area(face_ind)<<" nx "<<nx<<" ny "<<ny<<endl;
                //     cout<<"denom "<<denom_u<<" "<<denom_v<<endl;
                // }
                P_centers_copy[i-1] += area(face_ind) * P_centers_copy[out_center_ind-1] * nx;
                // P_centers[i-1] += area(face_ind) * P_centers[out_center_ind-1] * ny;
                key1.clear();
            }
            key.erase(key.end()-1);   
        }
        if(denom_y !=0){
            P_centers_copy[i-1]+= (P_centers_copy[i-1]/denom_y);
        }
        key.clear();
    }
    for(int i=0; i<center_num; i++){
        P_centers[i] = P_centers_copy[i];
    }
    for(int i=1; i<node_num+1; i++){
        if(find(nodes_boundary.begin(), nodes_boundary.end(), i) != nodes_boundary.end()){
            continue;
        }else{
            dP_dy_nodes[i-1] = (ro/delt) * cell_to_vertex_interpolation(i);
        }
    }
}
void neighbour_nodes(int k){ 
    // vector<int> nodes_ind; 
    set <int>::iterator s_it1 = l_node_to_nodes[k].begin(); 
    while(s_it1 != l_node_to_nodes[k].end()){ 
        cout << (*s_it1) << " " << nodes[*s_it1][0] << " " << nodes[*s_it1][1] << endl; 
        s_it1++; 

    } 
    cout << endl;
}
int main(){
    nodes_coor();//collecting coordinates and links
    cout<<"1"<<endl;
    cell_to_node();
    cout<<"2"<<endl;
    center_coordinates_generator();
    cout<<"3"<<endl;
    face_links();
    cout<<"4"<<endl;
    vector<double> v1;
    vector<int> v1_;
    for(int i=1; i<face_num+1; i++){
        v1 = normal(i);
        v1_.push_back(i);
        v1_.push_back(1);
        sn[v1_] = v1[0];
        v1_.erase(v1_.end()-1);
        v1_.push_back(2);
        sn[v1_] = v1[1];
        v1_.clear();
        v1.clear();
    }
    set_boundary_conditions();//finding l1
    jacobi();
    // set <int>::iterator s_it2 = m_it->first.begin();
    
    // neighbour_nodes(19);
    // neighbour_nodes(33);
    // neighbour_nodes(56);
    // neighbour_nodes(74);

    // cout<<"5"<<endl;

    // fvm_convection();
    // cout<<"6"<<endl;

    // for(int i=1; i<node_num+1; i++){
    //     if(find(nodes_boundary.begin(), nodes_boundary.end(), i) != nodes_boundary.end()){
    //         // cout<<i<<endl;
    //         continue;
    //     }else{
    //         P_nodes[i-1] = cell_to_vertex_interpolation(i);
    //     }
    // }
    // for(int i=0; i<node_num; i++){
    //     l1_nodes[i] = P_nodes[i];
    // }
    // for(int i=0; i<center_num; i++){
    //     l1_centers[i] = P_centers[i];
    // }
    // ofstream MFile("l1.dat");
    // MFile << "VARIABLES= \"X\", \"Y\", \"T\"" <<endl;
    // for(int i=0; i<node_num; i++){
    //     MFile << nodes[i+1][0] << " " << nodes[i+1][1] <<" "<<l1_nodes[i]<<endl;
    // }
    // MFile.close();

    // set_boundary_conditions_v();//finding l2
    // fvm_convection();
    // for(int i=1; i<node_num+1; i++){
    //     if(find(nodes_boundary.begin(), nodes_boundary.end(), i) != nodes_boundary.end()){
    //         // cout<<i<<endl;
    //         continue;
    //     }else{
    //         P_nodes[i-1] = cell_to_vertex_interpolation(i);
    //     }
    // }
    // for(int i=0; i<node_num; i++){
    //     l2_nodes[i] = P_nodes[i];
    // }
    // for(int i=0; i<center_num; i++){
    //     l2_centers[i] = P_centers[i];
    // }
    // ofstream MyFile("l2.dat");
    // MyFile << "VARIABLES= \"X\", \"Y\", \"T\"" <<endl;
    // for(int i=0; i<node_num; i++){
    //     MyFile << nodes[i+1][0] << " " << nodes[i+1][1] <<" "<<l2_nodes[i]<<endl;
    // }
    // MyFile.close();

    // for(int i=1; i<node_num+1; i++){
    //     if(find(nodes_boundary.begin(), nodes_boundary.end(), i) != nodes_boundary.end()){
    //         P_nodes[i-1] = 1;
    //     }else{
    //         P_nodes[i-1] = 0;
    //     }
    // }
    // u_star();
    // v_star();
    // ofstream yFile("u_star.dat");
    // yFile << "VARIABLES= \"X\", \"Y\", \"T\"" <<endl;
    // for(int i=0; i<node_num; i++){
    //     yFile << nodes[i+1][0] << " " << nodes[i+1][1] <<" "<<u_star_nodes[i]<<endl;
    // }
    // yFile.close();
    // ofstream RyFile("v_star.dat");
    // RyFile << "VARIABLES= \"X\", \"Y\", \"T\"" <<endl;
    // for(int i=0; i<node_num; i++){
    //     RyFile << nodes[i+1][0] << " " << nodes[i+1][1] <<" "<<v_star_nodes[i]<<endl;
    // }
    // RyFile.close();
    // set_boundary_conditions_P();
    // fvm_1st_derivatives();
    // cout<<"5"<<endl;
    // fdm_poisson();
    // cout<<"6"<<endl;

    // fvm_poisson();
    // find_P();
    // for(int i=1; i<node_num+1; i++){
    //     if(find(nodes_boundary.begin(), nodes_boundary.end(), i) != nodes_boundary.end()){
    //         // cout<<i<<endl;
    //         continue;
    //     }else{
    //         P_nodes[i-1] = cell_to_vertex_interpolation(i);
    //     }
    // }
    double max = 0;
    ofstream PyFile("Poisson.dat");
    PyFile << "VARIABLES= \"X\", \"Y\", \"K\"" <<endl;
    ofstream MFile("pressure_analitycal.dat");
    MFile << "VARIABLES= \"X\", \"Y\", \"P\"" <<endl;
    for(int i=0; i<node_num; i++){
        PyFile << nodes[i+1][0] << " " << nodes[i+1][1] <<" "<<P_nodes[i]<<endl;
        if(abs(sin(nodes[i+1][0] + 2 * nodes[i+1][1]) + exp(2 * nodes[i+1][0] + 3 * nodes[i+1][1]) - P_nodes[i]) > max){
            max = abs(sin(nodes[i+1][0] + 2 * nodes[i+1][1]) + exp(2 * nodes[i+1][0] + 3 * nodes[i+1][1])  - P_nodes[i]);
        }
        MFile << nodes[i+1][0] << " " << nodes[i+1][1] <<" "<<sin(nodes[i+1][0] + 2 * nodes[i+1][1]) + exp(2 * nodes[i+1][0] + 3 * nodes[i+1][1])<<endl;
    }
    cout<<"max error: "<<max<<endl;
    PyFile.close();
    MFile.close();
     
    // neighbour_nodes(0);
    // neighbour_nodes(20);
    // neighbour_nodes(99);
    // neighbour_nodes(79);
    // ofstream UyFile("u_n.dat");
    // UyFile << "VARIABLES= \"X\", \"Y\", \"P\"" <<endl;
    // for(int i=0; i<node_num; i++){
    //     UyFile << nodes[i+1][0] << " " << nodes[i+1][1] <<" "<<u_star_nodes[i] - ((delt/ro)*P_nodes[i]) <<endl;
    // }
    // UyFile.close();
    // ro_find();
    
    // for(int i=0; i<center_num; i++){
    //     MyFile << centers[i+1][0] << " " << centers[i+1][1] <<" "<<P_centers[i]<<endl;
    //     // cout<<i+1<< " "<<P_faces[i]<<endl;
    // }
    // for(int i=0; i<boundary_face.size(); i++){
    //     // MyFile <<P_faces[i]<<endl;
    //     cout<<i+1<< " "<<boundary_face[i]<<endl;
    // }
    // for(int i=0; i<node_num; i++){
    //     MyFile << nodes[i+1][0] << " " << nodes[i+1][1] <<" "<<P_nodes[i]/delt<<endl;
    // }
    // double P_nodes_old[node_num] = P_nodes;
    // fvm();
    // ofstream MFile("temperature_heat_time.dat");
    // MFile << "VARIABLES= \"X\", \"Y\", \"T\"" <<endl;
    // for(int i=0; i<node_num; i++){
    //     MFile << nodes[i+1][0] << " " << nodes[i+1][1] <<" "<<P_nodes_old[i]+(P_nodes[i]*delt)<<endl;
    // }
    // MFile.close();
    cout<< nodes[800][0]<< " "<<nodes[800][1]<<endl;
    return 0;
}