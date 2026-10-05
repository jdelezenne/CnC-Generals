#include "LegacyAssembly.h"
#include <algorithm>
#include <array>
#include <bit>
#include <cctype>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

namespace {
struct Opcode {unsigned Code,Operands;};
const std::unordered_map<std::string,Opcode> Opcodes{
    {"nop",{D3DSIO_NOP,0}},{"mov",{D3DSIO_MOV,2}},{"add",{D3DSIO_ADD,3}},{"sub",{D3DSIO_SUB,3}},
    {"mad",{D3DSIO_MAD,4}},{"mul",{D3DSIO_MUL,3}},{"rcp",{D3DSIO_RCP,2}},{"rsq",{D3DSIO_RSQ,2}},
    {"dp3",{D3DSIO_DP3,3}},{"dp4",{D3DSIO_DP4,3}},{"min",{D3DSIO_MIN,3}},{"max",{D3DSIO_MAX,3}},
    {"slt",{D3DSIO_SLT,3}},{"sge",{D3DSIO_SGE,3}},{"exp",{D3DSIO_EXP,2}},{"log",{D3DSIO_LOG,2}},
    {"lit",{D3DSIO_LIT,2}},{"dst",{D3DSIO_DST,3}},{"lrp",{D3DSIO_LRP,4}},{"frc",{D3DSIO_FRC,2}},
    {"m4x4",{D3DSIO_M4x4,3}},{"m4x3",{D3DSIO_M4x3,3}},{"m3x4",{D3DSIO_M3x4,3}},
    {"m3x3",{D3DSIO_M3x3,3}},{"m3x2",{D3DSIO_M3x2,3}},{"texcoord",{D3DSIO_TEXCOORD,1}},
    {"texkill",{D3DSIO_TEXKILL,1}},{"tex",{D3DSIO_TEX,1}},{"texbem",{D3DSIO_TEXBEM,2}},
    {"texbeml",{D3DSIO_TEXBEML,2}},{"texreg2ar",{D3DSIO_TEXREG2AR,2}},{"texreg2gb",{D3DSIO_TEXREG2GB,2}},
    {"texm3x2pad",{D3DSIO_TEXM3x2PAD,2}},{"texm3x2tex",{D3DSIO_TEXM3x2TEX,2}},
    {"texm3x3pad",{D3DSIO_TEXM3x3PAD,2}},{"texm3x3tex",{D3DSIO_TEXM3x3TEX,2}},
    {"texm3x3spec",{D3DSIO_TEXM3x3SPEC,3}},{"texm3x3vspec",{D3DSIO_TEXM3x3VSPEC,2}},
    {"expp",{D3DSIO_EXPP,2}},{"logp",{D3DSIO_LOGP,2}},{"cnd",{D3DSIO_CND,4}},{"def",{D3DSIO_DEF,5}}
};
unsigned Component(char ch)
{
    auto position=std::string_view("xyzw").find(ch);
    if(position==std::string_view::npos)position=std::string_view("rgba").find(ch);
    if(position==std::string_view::npos)throw std::runtime_error("Invalid register component");return static_cast<unsigned>(position);
}
unsigned Index(const std::string& text)
{
    size_t end=0;unsigned value=std::stoul(text,&end);
    if(end!=text.size())throw std::runtime_error("Invalid register index: "+text);return value;
}
DWORD Register(std::string text,bool destination)
{
    text.erase(std::remove_if(text.begin(),text.end(),[](unsigned char ch){return std::isspace(ch)!=0;}),text.end());
    DWORD modifier=0;
    if(!destination){
        if(text.starts_with("1-")){modifier=D3DSPSM_COMP;text.erase(0,2);}
        else if(text.starts_with('-')){modifier=D3DSPSM_NEG;text.erase(0,1);}
    }
    auto suffix=text.find('_');
    if(suffix!=std::string::npos){
        auto dot=text.find('.',suffix);auto name=text.substr(suffix,dot==std::string::npos?std::string::npos:dot-suffix);
        if(name=="_bias")modifier=modifier==D3DSPSM_NEG?D3DSPSM_BIASNEG:D3DSPSM_BIAS;
        else if(name=="_bx2")modifier=modifier==D3DSPSM_NEG?D3DSPSM_SIGNNEG:D3DSPSM_SIGN;
        else if(name=="_x2")modifier=modifier==D3DSPSM_NEG?D3DSPSM_X2NEG:D3DSPSM_X2;
        else throw std::runtime_error("Unknown register modifier: "+name);
        text.erase(suffix,name.size());
    }
    unsigned index=0;DWORD type=0,relative=0;
    std::string components;
    if(text.starts_with("c[")){
        auto end=text.find(']');if(end==std::string::npos)throw std::runtime_error("Unterminated constant register");
        auto expression=text.substr(2,end-2);
        if(expression.starts_with("a0.x")){relative=D3DVS_ADDRMODE_RELATIVE;expression.erase(0,4);if(expression.starts_with('+'))expression.erase(0,1);}
        index=expression.empty()?0:Index(expression);type=D3DSPR_CONST;
        if(end+1<text.size()){if(text[end+1]!='.')throw std::runtime_error("Invalid constant swizzle");components=text.substr(end+2);}
    }else{
        auto dot=text.find('.');auto name=text.substr(0,dot);if(dot!=std::string::npos)components=text.substr(dot+1);
        if(name=="opos")type=D3DSPR_RASTOUT;
        else if(name=="ofog"){type=D3DSPR_RASTOUT;index=1;}
        else if(name=="opts"){type=D3DSPR_RASTOUT;index=2;}
        else if(name.starts_with("od")){type=D3DSPR_ATTROUT;index=Index(name.substr(2));}
        else if(name.starts_with("ot")){type=D3DSPR_TEXCRDOUT;index=Index(name.substr(2));}
        else {
            if(name.size()<2)throw std::runtime_error("Invalid register: "+name);index=Index(name.substr(1));
            switch(name[0]){case 'r':type=D3DSPR_TEMP;break;case 'v':type=D3DSPR_INPUT;break;case 'c':type=D3DSPR_CONST;break;case 't':case 'a':type=D3DSPR_TEXTURE;break;default:throw std::runtime_error("Invalid register: "+name);}
        }
    }
    if(index>D3DSP_REGNUM_MASK || components.size()>4)throw std::runtime_error("Register outside supported range");
    DWORD token=0x80000000|type|index|relative;
    if(destination){
        DWORD mask=0;if(components.empty())mask=D3DSP_WRITEMASK_ALL;else for(char ch:components)mask|=1u<<(16+Component(ch));token|=mask;
    }else{
        DWORD swizzle=D3DSP_NOSWIZZLE;
        if(!components.empty()){swizzle=0;for(unsigned i=0;i<4;++i)swizzle|=Component(components[std::min<size_t>(i,components.size()-1)])<<(16+2*i);}
        token|=swizzle|modifier;
    }
    return token;
}
}
bool Platform::GPU::AssembleShader(std::string_view text,std::vector<DWORD>& code,std::string& error)
{
    unsigned lineNumber=0;
    try {
        code.clear();std::string clean;bool block=false;
        for(size_t i=0;i<text.size();++i){
            if(!block && i+1<text.size() && text[i]=='/' && text[i+1]=='*'){block=true;++i;continue;}
            if(block && i+1<text.size() && text[i]=='*' && text[i+1]=='/'){block=false;++i;continue;}
            if(block){if(text[i]=='\n')clean+='\n';continue;}
            if(text[i]==';' || (i+1<text.size() && text[i]=='/' && text[i+1]=='/')){while(i<text.size() && text[i]!='\n')++i;if(i<text.size())clean+='\n';continue;}
            clean+=static_cast<char>(std::tolower(static_cast<unsigned char>(text[i])));
        }
        std::istringstream input(clean);std::string line;
        while(std::getline(input,line)){
            ++lineNumber;std::istringstream stream(line);std::string name;if(!(stream>>name))continue;
            if(code.empty()){
                if(name!="vs.1.1" && name!="ps.1.1" && name!="ps.1.2" && name!="ps.1.3")throw std::runtime_error("Expected a shader model 1.x declaration");
                code.push_back(name[0]=='v'?D3DVS_VERSION(1,1):D3DPS_VERSION(1,name.back()-'0'));continue;
            }
            DWORD instruction=0,destinationModifier=0;
            if(name.front()=='+'){instruction=D3DSI_COISSUE;name.erase(0,1);if(name.empty() && !(stream>>name))throw std::runtime_error("Missing co-issued instruction");}
            for(auto suffix=name.find('_');suffix!=std::string::npos;suffix=name.find('_')){
                auto end=name.find('_',suffix+1);auto modifier=name.substr(suffix,end==std::string::npos?std::string::npos:end-suffix);
                if(modifier=="_sat")destinationModifier|=D3DSPDM_SATURATE;
                else if(modifier=="_x2")destinationModifier|=1u<<24;
                else if(modifier=="_x4")destinationModifier|=2u<<24;
                else if(modifier=="_x8")destinationModifier|=3u<<24;
                else if(modifier=="_d2")destinationModifier|=15u<<24;
                else if(modifier=="_d4")destinationModifier|=14u<<24;
                else if(modifier=="_d8")destinationModifier|=13u<<24;
                else throw std::runtime_error("Unknown instruction modifier "+modifier);
                name.erase(suffix,modifier.size());
            }
            auto found=Opcodes.find(name);if(found==Opcodes.end())throw std::runtime_error("Unknown shader instruction "+name);
            const auto op=found->second;code.push_back(instruction|op.Code);
            std::string operands;std::getline(stream,operands);std::istringstream fields(operands);std::vector<std::string> args;std::string arg;
            while(std::getline(fields,arg,','))args.push_back(arg);
            if(op.Operands==0 && args.size()==1 && args[0].find_first_not_of(" \t\r")==std::string::npos)args.clear();
            if(args.size()!=op.Operands)throw std::runtime_error("Wrong operand count for "+name);
            for(unsigned i=0;i<args.size();++i){
                if(op.Code==D3DSIO_DEF && i>0){size_t end=0;float value=std::stof(args[i],&end);if(args[i].find_first_not_of(" \t\r",end)!=std::string::npos)throw std::runtime_error("Invalid constant literal");code.push_back(std::bit_cast<DWORD>(value));}
                else code.push_back(Register(args[i],i==0)|(i==0?destinationModifier:0));
            }
        }
        if(code.empty())throw std::runtime_error("Empty shader source");code.push_back(D3DSIO_END);return true;
    }catch(const std::exception& exception){error="Shader line "+std::to_string(lineNumber)+": "+exception.what();code.clear();return false;}
}
