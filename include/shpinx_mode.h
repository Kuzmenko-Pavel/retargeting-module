#ifndef SHPINX_MODE_H
#define SHPINX_MODE_H

#include <string>

class shpinx_mode
{
    public:
        std::string match_;
        std::string rank_;
        std::string sort_;

        shpinx_mode();
        virtual ~shpinx_mode();
    protected:
    private:
};

#endif // SHPINX_MODE_H
