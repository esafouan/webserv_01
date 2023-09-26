#include "Request.hpp"

std::string Request::get_separater(std::string val)
{
    std::string sep = "";

    size_t pos = val.find("=");

    if (pos != std::string::npos)
        sep = val.substr(pos + 1);
    sep = "--" + sep;
    return sep;
}

void Request::get_post_status()
{
    this->lenght_of_content = std::atoi(valueOfkey("Content-Length", StoreHeaders).c_str());
    // std::cout << "content= " << valueOfkey("Content-Type", StoreHeaders) << "-"<<std::endl;
    this->extension = this->extensions[valueOfkey("Content-Type", StoreHeaders)];
    // std::cout << "ext= "<< this->extension << std::endl;
    if (find_key("Transfer-Encoding", StoreHeaders) && valueOfkey("Transfer-Encoding", StoreHeaders) == "chunked")
    {
        if (target.find(".php") != target.npos || target.find(".py") != target.npos)
            outfile_name = "directorie/upload/" + get_current_time() + ".txt";
        else
            outfile_name = "directorie/upload/" + get_current_time() + extension;

        outfile.open(outfile_name.c_str(), std::ios::binary);
        Post_status = "chunked";
    }

    else if (find_key("Content-Type", StoreHeaders) && valueOfkey("Content-Type", StoreHeaders).find("boundary") != std::string::npos)
    {
        Post_status = "boundary";
        if (target.find(".php") != target.npos)
        {
              outfile_name = "directorie/upload/" + get_current_time() + ".txt";
              Post_status = "Bainary/Row";
        }
          
        else
            boundary_separater = get_separater(valueOfkey("Content-Type", StoreHeaders));
    }

    else
    {
        if (target.find(".php") != target.npos || target.find(".py") != target.npos)
            outfile_name = "directorie/upload/" + get_current_time() + ".txt";
        else
            outfile_name = "directorie/upload/" + get_current_time() + extension;

        outfile.open(outfile_name.c_str(), std::ios::binary);
        Post_status = "Bainary/Row";
    }
}