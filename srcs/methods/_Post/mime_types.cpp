
#include "webserv.hpp"



std::string get_extention(Request &req){

    map<string, string> extentions;
    extentions["text/plain"] = ".txt";
    extentions["text/html"] = ".html";
    extentions["text/css"] = ".css";
    extentions["text/javascript"] = ".js";
    extentions["image/gif"] = ".gif";
    extentions["image/bmp"] = ".bmp";
    extentions["image/webp"] = ".webp";
    extentions["audio/wav"] = ".wav";
    extentions["audio/ogg"] = ".ogg";
    extentions["video/webm"] = ".webm";
    extentions["video/ogg"] = ".ogv";
    extentions["application/x-shockwave-flash"] = ".swf";
    extentions["application/epub+zip"] = ".epub";
    extentions["application/x-font-ttf"] = ".ttf";
    extentions["application/x-font-woff"] = ".woff";
    extentions["application/x-font-woff2"] = ".woff2";
    extentions["application/x-font-otf"] = ".otf";
    extentions["application/x-font-eot"] = ".eot";
    extentions["application/octet-stream"] = ".bin";
    extentions["application/x-font-svg"] = ".svg"; 
    extentions["application/json"] = ".json";
    extentions["application/xml"] = ".xml";
    extentions["image/jpeg"] = ".jpg";
    extentions["image/png"] = ".png";
    extentions["application/pdf"] = ".pdf";
    extentions["application/zip"] = ".zip";
    extentions["audio/mpeg"] = ".mp3";
    extentions["video/mp4"] = ".mp4";
    extentions["application/msword"] = ".doc";
    extentions["application/vnd.openxmlformats-officedocument.wordprocessingml.document"] = ".docx";
    extentions["application/vnd.ms-excel"] = ".xls";
    extentions["application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"] = ".xlsx";
    extentions["application/vnd.ms-powerpoint"] = ".ppt";
    extentions["application/vnd.openxmlformats-officedocument.presentationml.presentation"] = ".pptx";
    extentions["application/x-rar-compressed"] = ".rar";
    extentions["application/x-tar"] = ".tar";
    extentions["video/x-matroska"] = ".mkv";

    if (req.get_headers().find("Content-Type")->first.size() != 12) // 12 is length of "Content-Type"
        throw 400;
    string mime = trim_(req.get_header("Content-Type").substr(0, req.get_header("Content-Type").find(";")));
    std::string ext = extentions[mime];
    if (!ext.size())
        throw 415;
    return ext;
}
