#include<vector>
#include<string>
//#include "world_object.h"


#ifndef cell_class
#define cell_class
class cell //virtual base class, although not really nesessary, given that it has only two children...
{
    public:
        virtual ~cell(){};
        virtual std::string get_type()=0;
        virtual void cleanse()=0;
        
};

class water : public cell 
{
    private:
        std::string type;
    public:
        water(std::string entered_type) : type{entered_type} {};
        std::string get_type() {return type;};
        void cleanse() {};
};


class land : public cell 
{
    private:
        std::string type;
    public:
        land(std::string entered_type) : type{entered_type} {};
        std::string get_type() {return type;};
        void cleanse() {type = "land";};
};

#endif