//cell-centered SLE by Gauss-Seidel
//to do 
// add nfaces function 
// right order of nodes

#include <bits/stdc++.h>  
using namespace std;

const int face_num=180;
const int node_num=100;
const int center_num=81;
const double e=0.0001;

double gauss_seidel_coeff[center_num][center_num];
double gauss_seidel_b[center_num];


vector<vector<double> > nodes;
vector<vector<double> > centers;
map<vector<int> , int> l_cell_to_face;
map<vector<int> , int> l_face_to_cell;
map<vector<int> , int> l_face_to_node;
map<vector<int> , int> node_to_cell;
map<vector<int> , double> sn;
vector<double> P_nodes(node_num);
vector<double> P_faces(face_num);
vector<double> P_centers(center_num);
vector<double> P_centers_copy(center_num);


double find_max(vector<double> v){
    double max = v[0];
    for(int i=0; i<v.size(); i++){
        if(v[i]>max){
            max = v[i];
        }
    }
    return max;
}
double triangle_volume(vector<vector<double> > nodes_coor){
    double x1 = nodes_coor[0][0];
    double x2 = nodes_coor[1][0];
    double x3 = nodes_coor[2][0];
    double y1 = nodes_coor[0][1];
    double y2 = nodes_coor[1][1];
    double y3 = nodes_coor[2][1];
    return fabs(x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2;
}

double volume(int cell_ind){ 
    double V = 0;
    vector <int> key; 
    vector <int> key1; 
    vector<vector<double> > nodes_coor; 
    vector<vector<double> > nodes_coor2;
    key.push_back(cell_ind); 
    key.push_back(2); 
    int face_ind1 = l_cell_to_face[key];
    key.erase(key.end()-1); 
    key.push_back(4); 
    int face_ind2 = l_cell_to_face[key];

    key1.push_back(face_ind1);  
    key1.push_back(1);  
    int node_ind1 = l_face_to_node[key1]; 
    key1.erase(key1.end()-1);
    key1.push_back(2); 
    int node_ind2 = l_face_to_node[key1]; 
    key1.clear(); 

    key1.push_back(face_ind2);  
    key1.push_back(2);  
    int node_ind3 = l_face_to_node[key1]; 
    key1.erase(key1.end()-1);
    key1.push_back(1); 
    int node_ind4 = l_face_to_node[key1]; 
    key1.clear(); 
    nodes_coor.push_back(nodes[node_ind1]);
    nodes_coor.push_back(nodes[node_ind2]);
    nodes_coor.push_back(nodes[node_ind3]);
    nodes_coor.push_back(nodes[node_ind4]);  
    nodes_coor2.push_back(nodes_coor[0]); 

    
    for(int i = 1; i < nodes_coor.size()-1; i++){
        nodes_coor2.push_back(nodes_coor[i]);
        nodes_coor2.push_back(nodes_coor[i+1]);
        V += triangle_volume(nodes_coor2);
        nodes_coor2.erase(nodes_coor2.end()-1);
        nodes_coor2.erase(nodes_coor2.end()-1); 
    }

    return V;
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

vector<double> split_double(string line, char delimeter){
    string my_str = "";
    vector<double> v;
    for(int i; i<line.size(); i++){
        if((line[i] == delimeter) or (i==line.size()-1)){
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
            if(i==line.size()-1){
                my_str+=line[i];  
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

bool is_boundary_face(int face_ind){
    vector<int> key1;
    key1.push_back(face_ind);
    key1.push_back(1);
    vector<int> key2; 
    key2.push_back(face_ind);
    key2.push_back(2);
    //cout<<"!"<<key1[0]<<" "<<key1[1]<<" "<<key2[0]<<" "<<key2[1]<<" "<<endl;
    //cout<<"!"<<l_face_to_cell[key1]<<" "<<l_face_to_cell[key2]<<endl;
    if(l_face_to_cell[key1] == l_face_to_cell[key2]){
        return true;
    }
    return false;
}

// bool is_boundary_node(int node_ind){
//     if(node_ind in range(36)){
//         return true;
//     }
//     return false;
// }

// double distance(vector<double> p1, vector<double> p2){
//     // return pow((pow((p1[0] - p2[0]), 2) + pow((p1[1] - p2[1]), 2)) , 0.5);
//     return p1[0];
    
// }

vector<double> row_return(vector<vector<double> > v, int n){
    vector<double> row; 
    for(int i; i<v[n].size(); i++){
        row.push_back(v[n][i]);
    }
    return row;
}

// double distance(vector<double> p1, vector<double> p2){
//     double ans=0;
//     // for(int i=0; i<p1.size(); i++){
//     //     ans+= pow((p1[i]-p2[i]), 2);
//     // }
//     return pow(ans, 0.5);
// }

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

double cell_to_face_interpolation(int face_ind){
    if(is_boundary_face(face_ind)){
        return P_faces[face_ind];
    }

    //Collecting coordinates data
    vector <int> key; 
    key.push_back(face_ind); 
    key.push_back(1);
    double Pc1 = P_centers[l_face_to_cell[key]];
    vector<double> c1 = centers[l_face_to_cell[key]];
    vector<double> n1 = nodes[l_face_to_node[key]];
    key.erase(key.end()-1);
    key.push_back(2);
    double Pc2 = P_centers[l_face_to_cell[key]];
    vector<double> c2 = centers[l_face_to_cell[key]];
    vector<double> n2 = nodes[l_face_to_node[key]];
    vector<double> face_center_coor;
    face_center_coor.push_back((n1[0]+n2[0])/2);
    face_center_coor.push_back((n1[1]+n2[1])/2);

    //Serching for g
    double g = (1/(pow((pow((c1[0] - face_center_coor[0]), 2) + pow((c1[1] - face_center_coor[1]), 2)) , 0.5)))/((1/(pow((pow((c1[0] - face_center_coor[0]), 2) + pow((c1[1] - face_center_coor[1]), 2)) , 0.5))) + (1/(pow((pow((c2[0] - face_center_coor[0]), 2) + pow((c2[1] - face_center_coor[1]), 2)) , 0.5)))); 

    return (g*Pc1) +((1-g)*Pc2);
    // return distance(c1, face_center_coor);
    // return 0;
}

double cell_to_vertex_interpolation(int node_ind){ 
    vector<double> node_coor = nodes[node_ind]; 
    vector<int> key; 
    key.push_back(node_ind); 
    key.push_back(1); 
    int cell1 = node_to_cell[key]; 
    key.erase(key.end()-1);  
    key.push_back(2); 
    int cell2 = node_to_cell[key]; 
    key.erase(key.end()-1);  
    key.push_back(3); 
    int cell3 = node_to_cell[key]; 
    key.erase(key.end()-1);  
    key.push_back(4); 
    int cell4 = node_to_cell[key]; 
    cout << "Cells" << " " << cell1 << " " << cell2 << " " << cell3 << " " << cell4 << endl;
    vector<double> center1 = centers[cell1]; 
    vector<double> center2 = centers[cell2]; 
    vector<double> center3 = centers[cell3]; 
    vector<double> center4 = centers[cell4]; 
    double P1 = P_centers[cell1]; 
    double P2 = P_centers[cell2]; 
    double P3 = P_centers[cell3]; 
    double P4 = P_centers[cell4]; 
    double d1 = pow(pow((center1[0] - node_coor[0]), 2) + pow((center1[1] - node_coor[1]), 2), 0.5); 
    double d2 = pow(pow((center2[0] - node_coor[0]), 2) + pow((center2[1] - node_coor[1]), 2), 0.5); 
    double d3 = pow(pow((center3[0] - node_coor[0]), 2) + pow((center3[1] - node_coor[1]), 2), 0.5); 
    double d4 = pow(pow((center4[0] - node_coor[0]), 2) + pow((center4[1] - node_coor[1]), 2), 0.5); 
    cout << "P " << P1 << " " << P2 << " " << P3 << " " << P4 << endl;
    cout << "d " << d1 << " " << d2 << " " << d3 << " " << d4 << endl;
    return (P1/d1 + P2/d2 + P3/d3 + P4/d4)/(1/d1 + 1/d2 + 1/d3 + 1/d4); 
}

//FVM BY INTERPOLATION

// void fvm(int cell_ind){
//     double denominator = volume(cell_ind);
//     double nominator = 0;
//     //nfaces
//     vector<int> key; 
//     int face_int;
//     cout<<"denominator = "<<denominator<<endl;
//     key.push_back(cell_ind);
//     for(int i =1; i<5; i++){
//         key.push_back(i);
//         int face_ind = l_cell_to_face[key];
//         key.erase(key.end()-1);
//         nominator+= (cell_to_face_interpolation(face_ind) * area(face_ind));
//         cout<<face_ind<<" nominator = "<<nominator<<endl;

//     }
//     P_centers[cell_ind] = (nominator/denominator);
// }

void gauss_seidel(){
    // for(int i=0; i<center_num; i++){
    //     double temp = gauss_seidel_b[i];
    //     for(int j=0; j<center_num; j++){
    //         if(i!=j){
    //             temp -= gauss_seidel_coeff[i][j]*0;
    //         } 
    //     }
    //     P_centers[i] = (1/gauss_seidel_coeff[i][i]) * 0.8;
    //     P_centers_copy[i] = P_centers[i]; 
    //     cout<<i<<"$"<<endl;

    // }
    // for(int i=0; i<center_num; i++){
    //     cout<<P_centers[i]<<" ";
    //     if(i%9 == 8){
    //         cout<<endl;
    //     }
    // }
    // while(abs(find_max(P_centers_copy) - find_max(P_centers)) > e){
    // for(int m=0; m<100; m++){
    
    //     for(int i=0; i<center_num; i++){
    //         double temp = gauss_seidel_b[i];
    //         for(int j=0; j<center_num; j++){
    //             if(i!=j){
    //                 temp -= gauss_seidel_coeff[i][j]*P_centers[i];
    //             } 
    //         }
    //         P_centers[i] = (1/gauss_seidel_coeff[i][i]) * temp;
    //         P_centers_copy[i] = P_centers[i]; 
    //         cout<<i<<"$"<<endl;
    //     }
    // }
    double to_check = 1;
    // for(int m=0; m<50; m++){
    while(abs(to_check - find_max(P_centers))>e){
        to_check = find_max(P_centers);
        for(int i=0; i<center_num; i++){
        
            P_centers_copy[i] = (gauss_seidel_b[i] / gauss_seidel_coeff[i][i]);

            for(int j=0; j<center_num; j++){
                if (j == i){
                    continue;
                }
                P_centers_copy[i] = P_centers_copy[i] - ((gauss_seidel_coeff[i][j] / gauss_seidel_coeff[i][i]) * P_centers[j]);

                P_centers[i] = P_centers_copy[i];
            }
        }
        // cout<<m<<"it"<<endl;
        //     for(int k=0; k<3; k++){
        //         cout<<P_centers[k]<<" ";
        //     }
        //     cout<<endl;

    }

}

void jacobi(){
    double to_check = 1;
    while(abs(to_check - find_max(P_centers))>e){
    // for(int s=0; s<4; s++){
    
        to_check = find_max(P_centers);
        for(int m=0; m<center_num; m++){
            P_centers_copy[m] = P_centers[m];
        }
        for(int i=0; i<center_num; i++){
        // for(int i=center_num-1; i>=0; i--){
        
            P_centers[i] = gauss_seidel_b[i];
            for(int j=0; j<center_num; j++){
                if (j == i){
                    continue;
                }
                P_centers[i] -= (gauss_seidel_coeff[i][j] * P_centers_copy[j]);
                // cout<<P_centers_copy[j]<<" "<<j<<endl;
            }
            P_centers[i] /= gauss_seidel_coeff[i][i];

            // P_centers[i] = P_centers_copy[i];
        }
        // for(int i=0; i<center_num; i++){
        //     cout<<P_centers[i]<<" ";
        // }
        // cout<<endl;
    }
}

//FVM BY SYSTEM OF EQUATIONS

void fvm(){
    for(int i=0; i<center_num; i++){
    // for(int i=center_num-1; i>=0; i--){
        vector<double> gDiff;
        vector<int> key;
        key.push_back(i);
        double ac=0;
        double bc=0;
        vector<double> center_coor = centers[i];
        for(int j=1; j<5; j++){
            key.push_back(j);
            int face_ind = l_cell_to_face[key];
            double delt;
            if(is_boundary_face(face_ind)){
                vector<double> face_coor = face_center(face_ind);
                delt = pow((pow((face_coor[0] - center_coor[0]), 2) + pow((face_coor[1] - center_coor[1]), 2)) , 0.5);
                bc += (area(face_ind))*P_faces[face_ind]/delt;
                ac += (area(face_ind))/delt;
                gDiff.push_back((-1)*(area(face_ind))/delt);
                
                cout<<"face = "<<face_ind<<endl;
                cout<<"delt = "<<delt<<endl;
                cout<<"area = "<<area(face_ind)<<endl;
                cout<<"P = "<<P_faces[face_ind]<<endl;
                cout<<"   ac = "<<ac<<endl;
                cout<<"   a = "<<gDiff[gDiff.size()-1]<<endl;
                cout<<"   bc = "<<bc<<endl;

            }else{
                vector<int> key1;
                key1.push_back(face_ind);
                key1.push_back(1);
                vector<double> c1 = centers[l_face_to_cell[key1]];
                int out_center_ind = l_face_to_cell[key1]; 
                key1.erase(key1.end()-1);
                key1.push_back(2);
                vector<double> c2 = centers[l_face_to_cell[key1]];
                if(out_center_ind == i){
                    out_center_ind = l_face_to_cell[key1];
                }
                delt = pow((pow((c1[0] - c2[0]), 2) + pow((c1[1] - c2[1]), 2)) , 0.5);
                gauss_seidel_coeff[i][out_center_ind] = (-1)*(area(face_ind))/delt;
                ac += (area(face_ind))/delt;
                gDiff.push_back((-1)*(area(face_ind))/delt);
                cout<<"face = "<<face_ind<<endl;
                cout<<"delt = "<<delt<<endl;
                cout<<"area = "<<area(face_ind)<<endl;
                cout<<"   ac = "<<ac<<endl;
                cout<<"   a = "<<gDiff[gDiff.size()-1]<<endl;
                key1.clear();

            }
            // ac += (area(face_ind))/delt;
            // gDiff.push_back((-1)*(area(face_ind))/delt);
            key.erase(key.end()-1);
        }
        cout<<"ac = "<<ac<<endl;
        cout<<"bc = "<<bc<<endl;
        for(int m=1; m<gDiff.size()+1; m++){
            cout<<"a"<<m<<" = "<<gDiff[m-1]<<endl;
        }
        gauss_seidel_coeff[i][i] = ac;
        gauss_seidel_b[i] = bc;
        ac = 0;
        bc = 0; 
        key.clear();
        gDiff.clear();
        for(int s=0; s<center_num; s++){
            cout<<gauss_seidel_coeff[i][s]<<" ";
        }
        cout<<endl;
    }
    // for(int s=0; s<center_num; s++){
    // for(int l=0; l<center_num; l++){

    //         cout<<gauss_seidel_coeff[s][l]<<" ";
    //     }
    // }
    gauss_seidel();
    // jacobi();
}


int main(){

    //Mesh filling
    string myText;

        //Nodes filling
    ifstream NodesFile("nodes_square.txt");

    vector<double> v; 

    while (getline(NodesFile, myText)) {
        v = split_double(myText, ',');
        v.erase(v.begin());
        cout<<v[0]<<" "<<v[1]<<endl;
        nodes.push_back(v);
        v.clear();
    }

    NodesFile.close();

        //Centers filling
    ifstream CentersFile("centers_square.txt");

    while (getline(CentersFile, myText)) {
        v = split_double(myText, ',');
        v.erase(v.begin());
        cout<<v[0]<<" "<<v[1]<<endl;
        centers.push_back(v);
        v.clear();
    }

    CentersFile.close();

    cout<<centers[0][0]<< " "<<centers[centers.size()-1][1];

        //Cell to face connection filling
    ifstream Cell_to_face_File("l_cell_to_face.txt");

    vector<int> v1; 

    while (getline(Cell_to_face_File, myText)) {
        v1 = split_int(myText, ',');
        cout<<v1[0]<<" "<<v1[1]<<" "<<v1[2]<<endl;
        int temp = v1[v1.size()-1];
        v1.erase(v1.end()-1);
        l_cell_to_face[v1] = temp;
        v1.clear();
    }

    Cell_to_face_File.close();

        //Face to cell connection filling
    ifstream Face_to_cell_File("l_face_to_cell.txt");

    while (getline(Face_to_cell_File, myText)) {
        v1 = split_int(myText, ',');
        cout<<v1[0]<<" "<<v1[1]<<" "<<v1[2]<<endl;
        int temp = v1[v1.size()-1];
        v1.erase(v1.end()-1);
        l_face_to_cell[v1] = temp;
        v1.clear();
    }

    Face_to_cell_File.close();

        //Face to node connection filling
    ifstream Face_to_node_File("l_face_to_node.txt");

    while (getline(Face_to_node_File, myText)) {
        v1 = split_int(myText, ',');
        cout<<v1[0]<<" "<<v1[1]<<" "<<v1[2]<<endl;
        int temp = v1[v1.size()-1];
        v1.erase(v1.end()-1);
        l_face_to_node[v1] = temp;
        v1.clear();
    }

        //Node to cell coonection filling 

    ifstream Node_to_cell_File("node_to_cell.txt"); 
 
    while (getline(Node_to_cell_File, myText)) { 
        v1 = split_int(myText, ','); 
        cout<<v1[0]<<" "<<v1[1]<<" "<<v1[2]<<endl; 
        int temp = v1[v1.size()-1]; 
        v1.erase(v1.end()-1); 
        node_to_cell[v1] = temp; 
        v1.clear(); 
    } 
 
    Node_to_cell_File.close();

    Face_to_node_File.close();

        //Surface normal filling

    for(int i=0; i<180; i++){
        v = normal(i);
        v1.push_back(i);
        v1.push_back(1);
        sn[v1] = v[0];
        v1.erase(v1.end()-1);
        v1.push_back(2);
        sn[v1] = v[1];
        v1.clear();
        v.clear();
    }

    //Pressure filling
        //Nodes
    for(int i=0; i<node_num; i++){
        P_nodes.at(i) = 0;
    } 
    P_nodes.at(0)=1;
    int bound1=27;
    int bound2=36;
    int bound3[9] = {9, 28, 47, 66, 85, 104, 123, 142, 161}; //left 
    // int bound1=9;
    // int bound2=19;
    // int bound3[9] = {18, 37, 56, 75, 94, 113, 132, 151, 170};//right 
    // int bound1=18;
    // int bound2=28;
    // int bound3[9] = {171, 172, 173, 174, 175, 176, 177, 178, 179};//top
    // int bound1=0;
    // int bound2=10;
    // int bound3[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};//bottom
    for(int i=bound1; i<bound2; i++){
        P_nodes.at(i) = 1;
    } 
    
    //     //Outputting P_nodes
    // for(int i=0; i<node_num; i++){
    //     cout<<P_nodes.at(i)<<endl;
    // } 
        //Faces
    for(int i=0; i<face_num; i++){
        // vector <int> key; 
        // key.push_back(i); 
        // key.push_back(1);
        // int n1 = l_face_to_node[key];
        // key.erase(key.end()-1);
        // key.push_back(2);
        // int n2 = l_face_to_node[key];
        // if(P_nodes[n1] = P_nodes[n2]){
        //     P_faces[i]=1;
        // }else{
        //     P_faces[i]=0;
        // }
        P_faces[i]=0;

    }
    cout<<"BOUND"<<bound3[0]<<endl;
    for(int i=0; i<9; i++){
        P_faces.at(bound3[i]) = 1;
    } 

        //Outputting P_faces
    for(int i=0; i<face_num; i++){
        cout<<i<<" "<<P_faces.at(i)<<endl;
    } 
        //Centers
    for(int i=0; i<center_num; i++){
        P_centers.at(i)=0;
        P_centers_copy.at(i)=0;

    } 
    
    vector <double> key;
    vector <double> key1; 
    key.push_back(1); 
    key1.push_back(0); 
    key.push_back(1);
    key1.push_back(1);
    // // cout<<"!"<<key[0]<<" "<<key[1]<<" "<<key1[0]<<" "<<key1[1]<<" "<<endl;
    // // cout<<"!1"<<l_face_to_cell[key]<<endl;
    // // vector<double> n1 = normal(176);
    // cout<<sn[key]<<" "<<sn[key1]<<" "<<endl;
    // // cout<<is_boundary_face(2);
    // cout<<cell_to_face_interpolation(152);
    // cout << "volume = " << volume(0);
    // // fvm(0);
    // for(int i=0; i<center_num; i++){
    //     cout<<P_centers[i]<<" ";
    //     if(i%9 == 8){
    //         cout<<endl;
    //     }
    // }
    // for(int i=0; i<center_num; i++){
    //     fvm(i);
    // }
    // cout<<P_centers[0]<<endl;
    // for(int i=0; i<center_num; i++){
    //     cout<<P_centers[i]<<" ";
    //     if(i%9 == 8){
    //         cout<<endl;
    //     }
    // }
    fvm();
    cout<<P_centers[0]<<endl;
    for(int i=0; i<center_num; i++){
        cout<<P_centers[i]<<" ";
        if(i%9 == 8){
            cout<<endl;
        }
    }
    // cout<<cell_to_vertex_interpolation(36)<<endl;
    // fvm(72);

    return 0; 
}