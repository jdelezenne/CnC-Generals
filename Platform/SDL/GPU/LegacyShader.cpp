#include "LegacyShader.h"
#include "ShaderCompiler.h"
#include <SDL3/SDL.h>
#include <array>
#include <bit>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {
unsigned Arguments(unsigned opcode)
{
    switch (opcode) {
    case D3DSIO_NOP: case D3DSIO_PHASE: return 0;
    case D3DSIO_TEX: case D3DSIO_TEXCOORD: case D3DSIO_TEXKILL: return 1;
    case D3DSIO_MOV: case D3DSIO_RCP: case D3DSIO_RSQ: case D3DSIO_EXP:
    case D3DSIO_LOG: case D3DSIO_LIT: case D3DSIO_FRC: case D3DSIO_EXPP: case D3DSIO_LOGP:
    case D3DSIO_TEXBEM: case D3DSIO_TEXBEML: case D3DSIO_TEXREG2AR: case D3DSIO_TEXREG2GB:
    case D3DSIO_TEXM3x2PAD: case D3DSIO_TEXM3x2TEX: case D3DSIO_TEXM3x3PAD:
    case D3DSIO_TEXM3x3TEX: case D3DSIO_TEXM3x3VSPEC: return 2;
    case D3DSIO_MAD: case D3DSIO_LRP: case D3DSIO_CND: case D3DSIO_CMP: return 4;
    case D3DSIO_DEF: return 5;
    case D3DSIO_ADD: case D3DSIO_SUB: case D3DSIO_MUL: case D3DSIO_DP3: case D3DSIO_DP4:
    case D3DSIO_MIN: case D3DSIO_MAX: case D3DSIO_SLT: case D3DSIO_SGE: case D3DSIO_DST:
    case D3DSIO_M4x4: case D3DSIO_M4x3: case D3DSIO_M3x4: case D3DSIO_M3x3: case D3DSIO_M3x2:
        return 3;
    case D3DSIO_TEXM3x3SPEC: return 3;
    default: throw std::runtime_error("Unsupported shader opcode " + std::to_string(opcode));
    }
}
std::string Number(float value)
{
    std::ostringstream stream;
    stream << std::setprecision(9) << std::scientific << value;
    return stream.str();
}
struct Translator {
    bool Vertex;
    std::ostringstream Body;
    unsigned Instruction = 0;
    std::string Register(DWORD token)
    {
        unsigned index=token&D3DSP_REGNUM_MASK;
        auto name=std::to_string(index);
        switch (token&D3DSP_REGTYPE_MASK) {
        case D3DSPR_TEMP: if(index>=12)break; return "r["+name+"]";
        case D3DSPR_INPUT: if(index>=(Vertex?16u:2u))break; return "v["+name+"]";
        case D3DSPR_CONST:
            if(index>=96)break;
            return std::string("c[")+((token&D3DVS_ADDRESSMODE_MASK)?"(int)a.x+":"")+name+"]";
        case D3DSPR_TEXTURE:
            if(Vertex)return "a";
            if(index<8)return "t["+name+"]";
            break;
        case D3DSPR_RASTOUT:
            if(index==0)return "position";
            if(index==1)return "fog";
            if(index==2)return "pointSize";
            break;
        case D3DSPR_ATTROUT: if(index<2)return "color["+name+"]"; break;
        case D3DSPR_TEXCRDOUT: if(index<8)return "uv["+name+"]"; break;
        }
        throw std::runtime_error("Invalid shader register");
    }
    std::string Source(DWORD token)
    {
        std::string value=Register(token);
        std::string swizzle;
        for(unsigned i=0;i<4;++i)swizzle+="xyzw"[(token>>(16+2*i))&3];
        value="("+value+"."+swizzle+")";
        switch(token&D3DSP_SRCMOD_MASK) {
        case D3DSPSM_NONE: break;
        case D3DSPSM_NEG: value="(-"+value+")"; break;
        case D3DSPSM_BIAS: value="("+value+"-0.5)"; break;
        case D3DSPSM_BIASNEG: value="(0.5-"+value+")"; break;
        case D3DSPSM_SIGN: value="(2*"+value+"-1)"; break;
        case D3DSPSM_SIGNNEG: value="(1-2*"+value+")"; break;
        case D3DSPSM_COMP: value="(1-"+value+")"; break;
        case D3DSPSM_X2: value="(2*"+value+")"; break;
        case D3DSPSM_X2NEG: value="(-2*"+value+")"; break;
        case D3DSPSM_DZ: value="("+value+"/"+Register(token)+".z)"; break;
        case D3DSPSM_DW: value="("+value+"/"+Register(token)+".w)"; break;
        default: throw std::runtime_error("Unsupported shader source modifier");
        }
        return value;
    }
    std::string Mask(DWORD token)
    {
        std::string result;
        for(unsigned i=0;i<4;++i)if(token&(1u<<(16+i)))result+="xyzw"[i];
        if(result.empty())throw std::runtime_error("Empty shader destination mask");
        return result;
    }
    std::string Evaluate(unsigned op, const DWORD* p)
    {
        auto src=[&](unsigned index){return Source(p[index]);};
        auto s=Arguments(op)>1?src(1):std::string{};
        auto b=Arguments(op)>2?src(2):std::string{};
        auto d=Arguments(op)>3?src(3):std::string{};
        switch(op) {
        case D3DSIO_MOV: return s;
        case D3DSIO_ADD: return s+"+"+b;
        case D3DSIO_SUB: return s+"-"+b;
        case D3DSIO_MUL: return s+"*"+b;
        case D3DSIO_MAD: return s+"*"+b+"+"+d;
        case D3DSIO_DP3: return "dot("+s+".xyz,"+b+".xyz).xxxx";
        case D3DSIO_DP4: return "dot("+s+","+b+").xxxx";
        case D3DSIO_MIN: return "min("+s+","+b+")";
        case D3DSIO_MAX: return "max("+s+","+b+")";
        case D3DSIO_SLT: return "float4("+s+"<"+b+")";
        case D3DSIO_SGE: return "float4("+s+">="+b+")";
        case D3DSIO_RCP: return "(1.0/"+s+".x).xxxx";
        case D3DSIO_RSQ: return "rsqrt(abs("+s+".x)).xxxx";
        case D3DSIO_EXP: return "exp2("+s+".x).xxxx";
        case D3DSIO_LOG: return "log2(abs("+s+".x)).xxxx";
        case D3DSIO_EXPP: return "float4(exp2(floor("+s+".x)),frac("+s+".x),exp2("+s+".x),1)";
        case D3DSIO_LOGP: return "float4(floor(log2(abs("+s+".x))),abs("+s+".x)/exp2(floor(log2(abs("+s+".x)))),log2(abs("+s+".x)),1)";
        case D3DSIO_LIT: return "float4(1,max("+s+".x,0),"+s+".x>0?pow(max("+s+".y,0),clamp("+s+".w,-127.9961,127.9961)):0,1)";
        case D3DSIO_DST: return "float4(1,"+s+".y*"+b+".y,"+s+".z,"+b+".w)";
        case D3DSIO_LRP: return "("+s+"*"+b+"+(1-"+s+")*"+d+")";
        case D3DSIO_FRC: return "frac("+s+")";
        case D3DSIO_CND: return "("+s+".a>0.5?"+b+":"+d+")";
        case D3DSIO_CMP: return "select("+s+">=0,"+b+","+d+")";
        case D3DSIO_M4x4: case D3DSIO_M4x3: case D3DSIO_M3x4: case D3DSIO_M3x3: case D3DSIO_M3x2: {
            unsigned rows=op==D3DSIO_M4x4 || op==D3DSIO_M3x4?4:op==D3DSIO_M3x2?2:3;
            bool three=op==D3DSIO_M3x4 || op==D3DSIO_M3x3 || op==D3DSIO_M3x2;
            std::string result="float4(";
            for(unsigned i=0;i<4;++i){if(i)result+=",";result+=i<rows?"dot("+s+(three?".xyz":"")+","+Source(p[2]+i)+(three?".xyz":"")+")":"0";}
            return result+")";
        }
        case D3DSIO_TEX: return "Sample("+std::to_string(p[0]&D3DSP_REGNUM_MASK)+",t["+std::to_string(p[0]&D3DSP_REGNUM_MASK)+"].xy)";
        case D3DSIO_TEXCOORD: return "saturate(input.uv"+std::to_string(p[0]&D3DSP_REGNUM_MASK)+")";
        case D3DSIO_TEXBEM: case D3DSIO_TEXBEML: {
            auto stage=std::to_string(p[0]&D3DSP_REGNUM_MASK);
            auto value="Sample("+stage+",input.uv"+stage+".xy+float2(dot("+s+".xy,bump["+stage+"].xy),dot("+s+".xy,bump["+stage+"].zw)))";
            if(op==D3DSIO_TEXBEML)value+="*float4(("+s+".z*luminance["+stage+"].x+luminance["+stage+"].y).xxx,1)";
            return value;
        }
        case D3DSIO_TEXREG2AR: return "Sample("+std::to_string(p[0]&D3DSP_REGNUM_MASK)+","+s+".wx)";
        case D3DSIO_TEXREG2GB: return "Sample("+std::to_string(p[0]&D3DSP_REGNUM_MASK)+","+s+".yz)";
        default: throw std::runtime_error("Unsupported shader instruction " + std::to_string(op));
        }
    }
    void Emit(DWORD instruction,const DWORD* p)
    {
        unsigned op=instruction&D3DSI_OPCODE_MASK;
        if(op==D3DSIO_NOP)return;
        if(op==D3DSIO_DEF){Body<<Register(p[0])<<"=float4(";for(unsigned i=1;i<5;++i){if(i>1)Body<<",";Body<<Number(std::bit_cast<float>(p[i]));}Body<<");\n";return;}
        if(op==D3DSIO_TEXKILL){Body<<"if(any("<<Register(p[0])<<".xyz<0))discard;\n";return;}
        auto mask=Mask(p[0]);
        auto variable="value"+std::to_string(Instruction++);
        std::string result="("+Evaluate(op,p)+")";
        int shift=(p[0]>>24)&15;if(shift>7)shift-=16;
        if(shift)result="("+result+"*"+Number(std::ldexp(1.0f,shift))+")";
        if(p[0]&D3DSPDM_SATURATE)result="saturate("+result+")";
        if(Vertex && (p[0]&D3DSP_REGTYPE_MASK)==D3DSPR_ADDR)result="floor("+result+"+0.5)";
        // Co-issued instructions evaluate from the same register values.
        Body<<"float4 "<<variable<<"="<<result<<";\n";
        Pending=Register(p[0])+"."+mask+"="+variable+"."+mask+";\n";
    }
    std::string Pending;
};
}

bool Platform::GPU::ReadShader(const DWORD* code,std::vector<DWORD>& result,std::string& error)
{
    if(!code){error="Null shader program";return false;}
    try {
        result.clear();result.push_back(*code++);
        if((result[0]&0xff00)!=0x100 || (result[0]&255)>4)throw std::runtime_error("Expected shader model 1.x");
        for(unsigned words=0;words<16384;){
            DWORD instruction=*code++;result.push_back(instruction);++words;
            unsigned op=instruction&D3DSI_OPCODE_MASK;
            if(op==D3DSIO_END)return true;
            unsigned count=op==D3DSIO_COMMENT?((instruction>>16)&32767):Arguments(op);
            if(words+count>16384)throw std::runtime_error("Shader exceeds token limit");
            result.insert(result.end(),code,code+count);code+=count;words+=count;
        }
        throw std::runtime_error("Unterminated shader program");
    } catch(const std::exception& exception){error=exception.what();result.clear();return false;}
}
bool Platform::GPU::TranslateShader(const std::vector<DWORD>& code,bool vertex,std::string& source,std::string& error)
{
    try {
        if(code.empty() || (code[0]>>16)!=(vertex?0xfffe:0xffff))throw std::runtime_error("Shader stage does not match program");
        if((code[0]&255)>3 && !vertex)throw std::runtime_error("Pixel shader 1.4 is not implemented");
        Translator translator{vertex};
        std::ostringstream header;
        header<<"struct Varyings { float4 position:SV_Position; float4 diffuse:COLOR0; float4 specular:COLOR1;";
        for(unsigned i=0;i<8;++i)header<<"float4 uv"<<i<<":TEXCOORD"<<i<<";";
        header<<"float fog:TEXCOORD8; };\n";
        if(vertex){
            header<<"struct Input {";for(unsigned i=0;i<16;++i)header<<"float4 v"<<i<<":TEXCOORD"<<i<<";";header<<"};\n";
            header<<"cbuffer Constants:register(b0,space1){float4 constants[96];float4 viewport;};\n";
        }else{
            for(unsigned i=0;i<8;++i)header<<"Texture2D<float4> texture"<<i<<":register(t"<<i<<",space2);SamplerState sampler"<<i<<":register(s"<<i<<",space2);\n";
            header<<"cbuffer Constants:register(b0,space3){float4 constants[96];float4 viewport;float4 bump[8];float4 luminance[8];float4 alpha;float4 fogColor;float4 fogParameters;int4 flags;};\n";
            header<<"float4 Sample(int stage,float2 uv){switch(stage){";
            for(unsigned i=0;i<8;++i)header<<"case "<<i<<":return texture"<<i<<".Sample(sampler"<<i<<",uv);";
            header<<"}return 0;}\n";
        }
        header<<(vertex?"Varyings main(Input input){":"float4 main(Varyings input):SV_Target0{");
        header<<"float4 r[12]=(float4[12])0;float4 c[96];for(int i=0;i<96;++i)c[i]=constants[i];float4 v["<<(vertex?16:2)<<"];";
        if(vertex){for(unsigned i=0;i<16;++i)header<<"v["<<i<<"]=input.v"<<i<<";";header<<"float4 a=0,position=0,fog=1,pointSize=1;float4 color[2]=(float4[2])0;float4 uv[8]=(float4[8])0;";}
        else{header<<"v[0]=input.diffuse;v[1]=input.specular;float4 t[8];";for(unsigned i=0;i<8;++i)header<<"t["<<i<<"]=input.uv"<<i<<";";}
        for(size_t i=1;i<code.size();){
            DWORD instruction=code[i++];unsigned op=instruction&D3DSI_OPCODE_MASK;if(op==D3DSIO_END)break;
            if(op==D3DSIO_COMMENT){i+=(instruction>>16)&32767;continue;}
            unsigned count=Arguments(op);if(i+count>code.size())throw std::runtime_error("Truncated shader instruction");
            if(!(instruction&D3DSI_COISSUE)){translator.Body<<translator.Pending;translator.Pending.clear();}
            auto previous=translator.Pending;translator.Emit(instruction,code.data()+i);
            if(instruction&D3DSI_COISSUE){translator.Body<<previous<<translator.Pending;translator.Pending.clear();}
            i+=count;
        }
        header<<translator.Body.str()<<translator.Pending;
        if(vertex){
            header<<"Varyings output;position.xy+=float2(1/viewport.x,-1/viewport.y)*position.w;output.position=position;output.diffuse=saturate(color[0]);output.specular=saturate(color[1]);";
            for(unsigned i=0;i<8;++i)header<<"output.uv"<<i<<"=uv["<<i<<"];";
            header<<"output.fog=fog.x;return output;}";
        }else{
            header<<"float4 result=saturate(r[0]);if(flags.x!=0){float a=round(result.a*255),b=alpha.x;bool pass=true;switch(flags.y){case 1:pass=false;break;case 2:pass=a<b;break;case 3:pass=a==b;break;case 4:pass=a<=b;break;case 5:pass=a>b;break;case 6:pass=a!=b;break;case 7:pass=a>=b;break;}if(!pass)discard;}";
            header<<"if(flags.w!=0){float f=input.fog;float d=alpha.y!=0?input.position.z:input.fog;int mode=(int)fogParameters.w;if(mode==1)f=exp(-fogParameters.z*d);if(mode==2)f=exp(-pow(fogParameters.z*d,2));if(mode==3)f=(fogParameters.y-d)/(fogParameters.y-fogParameters.x);result.rgb=lerp(fogColor.rgb,result.rgb,saturate(f));}return result;}";
        }
        source=header.str();return true;
    } catch(const std::exception& exception){error=exception.what();return false;}
}
SDL_GPUShader* Platform::GPU::CompileShader(SDL_GPUDevice* device,const std::string& source,bool vertex,bool spirv)
{
    std::vector<std::uint8_t> bytecode;
    std::string error;
    if(!CompileHLSL(source,vertex,spirv,bytecode,error)){SDL_SetError("Legacy shader compilation: %s",error.c_str());return nullptr;}
    SDL_GPUShaderCreateInfo info{};info.code_size=bytecode.size();info.code=bytecode.data();info.entrypoint="main";
    info.format=spirv?SDL_GPU_SHADERFORMAT_SPIRV:SDL_GPU_SHADERFORMAT_DXIL;info.stage=vertex?SDL_GPU_SHADERSTAGE_VERTEX:SDL_GPU_SHADERSTAGE_FRAGMENT;
    info.num_samplers=vertex?0:8;info.num_uniform_buffers=1;
    return SDL_CreateGPUShader(device,&info);
}
