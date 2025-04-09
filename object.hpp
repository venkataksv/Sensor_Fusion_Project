#include <iostream>
#include <string>

class cObject {
    public:
        int cam_id;
        int frame_id;
        std::string timestamp;
        int area;
        std::string object_classes;
        std::vector<int> bboxes;
        std::vector<int> object_positions_x_y;
        int object_area;
        std::vector<double> object_confidences;
        int object_association_ids;
        std::vector<bool> vest_flags;
};