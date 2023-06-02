# include<ros/ros.h>
# include<nav_msgs/OccupancyGrid.h>
# include <iostream>
# include<cstdlib>
# include<algorithm>
void shrink_race_track(int **map, int rows, int cols, int index){
    for (int i=0; i<rows; i++){
        for (int j=0; j<cols; j++){
            bool have_at_least_one_neigb_occ = false;
            int this_grid_occ = map[i][j];
            if (this_grid_occ == 1){
                continue;  // 已经是障碍物了，不需要shrink
            }
            for (int l=i-1; l<=i+1; l++){
                if (l<0 || l>=rows)
                    continue;
                for (int m=j-1; m<=j+1; m++){
                    if (m<0 || m>=cols)
                        continue;
                    if (l==i && m==j)
                        continue;
                    int this_neighbor_occ = map[l][m];
                    if (this_neighbor_occ == 1){
                        have_at_least_one_neigb_occ = true;
                        break;
                    }
                }
                if (have_at_least_one_neigb_occ)
                    break;
            }
            // 根据have_at_least_one_neigb_occ的值决定膨胀与否
            if (have_at_least_one_neigb_occ){
                for (int l=i-index; l<=i+index; l++){
                    if (l<0 || l>=rows)
                        continue;
                    for (int m=j-index; m<=j+index; m++){
                        if (m<0 || m>=cols)
                            continue;
                        map[l][m] = 1;
                    }
                }
            }
            else {
                map[i][j] = 0;
            }
        }
    }
}


int **shrink_race_track(int **map, int rows, int cols, int index){

    if(map==nullptr||rows<=0||cols<=0||index<1)
    {
        return nullptr;
    }

    int **new_map = new int*[rows];

    for (int i=0; i<rows; i++){
        new_map[i] = new int[cols];
        
        memset(new_map[i],0,cols*sizeof(int));

        for (int j=0; j<cols; j++){
            bool have_at_least_one_neigb_occ = false;
            int this_grid_occ = map[i][j];
            if (this_grid_occ == 1){
                new_map[i][j]=1;
                continue;

            }

           
            for (int l=std::max(i-1,0); l<=std::min(i+1,rows-1); l++){
                for (int m=std::max(j-1,0); m<=std::min(j+1,cols-1); m++){
                    int this_neighbor_occ = map[l][m];
                    if (this_neighbor_occ == 1){
                        have_at_least_one_neigb_occ = true;
                        goto end_loop;
                    }
                }
            }

            end_loop:
            // 根据have_at_least_one_neigb_occ的值决定膨胀与否
 
            if (have_at_least_one_neigb_occ){

                int start_l=std::max(i-index,0);
                int end_l=std::min(i+index,rows-1);
                int start_m=std::max(j-index,0);
                int end_m=std::min(j+index,cols-1);
                printf("bbbsdf\n");
                for (int l=start_l; l<=end_l; l++){
                    printf("cccsdf\n");
                    for (int m=start_m; m<=end_m; m++){
                        if(l>=0 && m>=0 && l<rows && m<cols){
                            new_map[l][m] = 1;
                        }
                        
                    }
                }
                printf("bbbsdf\n");
            }
            else {
                if(i>=0 && j>=0 && i<rows && j<cols){
                    new_map[i][j] = 0;
                }
            }
        }
    }
    
    return new_map;
}

void add_random_obstacles(int **map, int rows, int cols, int obstacle_wide, int car_wide, int safe_distance, int obstacle_num_max){
    int obstacle_num = 0;
    int row, col;
    while (obstacle_num < obstacle_num_max){
        row = rand() % rows;   // 随机选取一个位置
        col = rand() % cols;

        if (col < 100)     // 不能生成在初始位置
            continue;

        int safe_wide = obstacle_wide + car_wide + safe_distance;
        bool have_at_least_one_neigb_occ = false;    // 有一个被占据说明不安全，无法生成
        for (int l=row-safe_wide; l<=row+safe_wide; l++){
            if (l<0 || l>=rows)
                continue;
            for (int m=col-safe_wide; m<=col+safe_wide; m++){
                if (m<0 || m>=cols)
                    continue;
                int this_neighbor_occ = map[l][m];
                if (this_neighbor_occ == 1){
                    have_at_least_one_neigb_occ = true;
                    break;
                }
            }
            if (have_at_least_one_neigb_occ)
                break;
        }
        if (!have_at_least_one_neigb_occ){
            for (int l=row-obstacle_wide; l<=row+obstacle_wide; l++){
                if (l<0 || l>=rows)
                    continue;
                for (int m=col-obstacle_wide; m<=col+obstacle_wide; m++){
                    if (m<0 || m>=cols)
                        continue;
                    map[l][m] = 1;
                }
            }
            obstacle_num = obstacle_num + 1;
        }
    }
    
}

void visualize_map(const ros::Publisher& pub, int **map, int rows, int cols){
    while (ros::ok()){
    nav_msgs::OccupancyGrid map_msg;
    map_msg.header.frame_id = "map";
    map_msg.info.resolution = 0.01;
    map_msg.info.width = cols;
    map_msg.info.height = rows;
    map_msg.info.origin.position.x = -5.0;
    map_msg.info.origin.position.y = -5.0;
    map_msg.info.origin.orientation.w = 0.0;

    std::vector<int8_t> data(rows*cols);
    for (int i=0; i<rows; i++){
        for (int j=0; j<cols; j++){
            int index = cols*i + j;
            if (map[i][j] == 1)
                data[index] = 100;
            else
                data[index] = 0;
        }

    }
    map_msg.data = data;
    pub.publish(map_msg);
    }


 
}

void map_callback(const nav_msgs::OccupancyGrid::ConstPtr& msg){
    int rows = msg->info.height;
    int cols = msg->info.width;

    int **map = new int*[rows];
    for (int i=0; i<rows; i++){
        map[i] = new int[cols];
        for (int j=0; j<cols; j++){
            int index = cols*i + j;
            if (msg->data[index] == -1){
                map[i][j] = 0;  // 未知空间
            }
            else if (msg->data[index] == 100){
                map[i][j] = 1;  // 障碍物
            }
            else {
                map[i][j] = 0;  // 自由空间
            }
        }
    }

    // 对地图进行处理并生成新的地图
   
    int index = 5;
    map = shrink_race_track(map, rows, cols, index);

    int obstacle_wide = 2;
    int car_wide = 3;
    int safe_distance = 1;
    int obstacle_num_max = 5;
    
    add_random_obstacles(map, rows, cols, obstacle_wide, car_wide, safe_distance, obstacle_num_max);

    // 可视化新的地图
    ros::NodeHandle nh;
    ros::Publisher pub = nh.advertise<nav_msgs::OccupancyGrid>("visualized_map", 100);
    
    visualize_map(pub, map, rows, cols);

    for(int i=0;i<rows;i++){
        delete[]map[i];
    }
    delete[]map;
}

int main(int argc, char **argv){
    ros::init(argc, argv, "map_pub_node");
    ros::NodeHandle nh;
    ros::Rate r(1);
    ros::Subscriber sub = nh.subscribe("map", 1000, map_callback);
    while(ros::ok())
    {   
        
     
        
        ros::spinOnce();
        
        
    }
    r.sleep();

    return 0;
}
/*

int main(int argc, char* argv[])
{
    ros::init(argc,argv,"map_pub_node");

    ros::NodeHandle n;
    ros::Publisher pub =n.advertise<nav_msgs::OccupancyGrid>("/map",10);
    
    ros::Rate r(1);


    while(ros::ok())
    {
        nav_msgs::OccupancyGrid msg;
        msg.header.frame_id="map";
        msg.header.stamp=ros::Time::now();

        msg.info.origin.position.x=0;
        msg.info.origin.position.y=0;
        msg.info.resolution=1.0;
        msg.info.width=40;
        msg.info.height=20;
        
        msg.data.resize(40*20);
        for(int i=0;i<40*20;i++)
        {
            if(i%7==0){
                msg.data[i]=100;
            }
      
        }

    
    pub.publish(msg);
    r.sleep();
    }
    return 0;
}
*/
