#pragma once

"#ifdef VERTEX\n"
"layout(location=0) in vec2 Position;"
"layout(location=1) in vec2 TexCoord;"
"layout(location=4) in ivec2 TexIds;"
"layout(location=5) in uvec4 Flags;"
"uniform mat4 u_MVP;"
"out vec2 v_TexCoord;"
"flat out ivec2 v_TexIds;"
"flat out uvec4 v_Flags;"
"void main()"
"{"
  "gl_Position=u_MVP*vec4(Position,0,1);"
  "v_TexCoord=TexCoord;"
  "v_TexIds=TexIds;"
  "v_Flags=Flags;"
"}"
"\n#elif FRAGMENT\n"
"layout(location=0) out vec4 FragColor;"
"layout(location=1) out vec4 FragColorMap;"
"layout(location=2) out vec4 FragColorMask;layout(std140) uniform ubo_Metrics{float u_BloomExp;float u_BloomGamma;vec2 u_RelSize;};"
"uniform sampler2D u_Texture,u_BloomTexture1,u_BloomTexture2;"
"uniform sampler2DArray u_LUTTexture;"
"in vec2 v_TexCoord;"
"flat in ivec2 v_TexIds;"
"flat in uvec4 v_Flags;"
"\n#define CoefLuma vec3(0.2126,0.7152,0.0722)\n"
"vec4 t()"
"{"
  "vec3 v=texture(u_Texture,v_TexCoord).xyz;"
  "float u=dot(v,CoefLuma);"
  "return u>.97?"
    "(v=mix(vec3(dot(v,CoefLuma)),v,50.),vec4(v,1)):"
    "vec4(0);"
"}"
"vec4 t(sampler2D v,int u)"
"{"
  "vec4 i=vec4(0);"
  "vec2 F[13];"
  "if(u==0)"
    "for(int e=-6;e<=6;e++)"
      "F[e+6]=v_TexCoord+vec2(u_RelSize.x*e,0);"
  "else "
     "for(int e=-5;e<=6;e++)"
      "F[e+6]=v_TexCoord+vec2(0,u_RelSize.y*e);"
  "i+=texture(v,F[0])*.000244140625;"
  "i+=texture(v,F[1])*.0029296875;"
  "i+=texture(v,F[2])*.01611328125;"
  "i+=texture(v,F[3])*.0537109375;"
  "i+=texture(v,F[4])*.120849609375;"
  "i+=texture(v,F[5])*.193359375;"
  "i+=texture(v,F[6])*.2255859375;"
  "i+=texture(v,F[7])*.193359375;"
  "i+=texture(v,F[8])*.120849609375;"
  "i+=texture(v,F[9])*.0537109375;"
  "i+=texture(v,F[10])*.01611328125;"
  "i+=texture(v,F[11])*.0029296875;"
  "i+=texture(v,F[12])*.000244140625;"
  "return i;"
"}"
"vec4 t(vec4 v,vec4 u,float F)"
"{"
  "return v.z<1.?"
    "mix(v,u,F):"
    "v;"
"}"
"\n#define LUT_Size 32.0\n"
"vec4 t(vec4 v)"
"{"
  "float u=(v.x*(LUT_Size-1.)+.4999)/(LUT_Size*LUT_Size),F=(v.y*(LUT_Size-1.)+.4999)/LUT_Size,e=floor(v.z*(LUT_Size-1.))/LUT_Size+u,l=ceil(v.z+1e-6*(LUT_Size-1.))/LUT_Size+u,n=clamp(max((v.z-e)/(l-e),0.),0.,32.);"
  "vec4 i=texture(u_LUTTexture,vec3(vec2(e,F),v_TexIds.x-1)),r=texture(u_LUTTexture,vec3(vec2(l,F),v_TexIds.x-1));"
  "return t(i,r,n);"
"}"
"void main()"
"{"
  "switch(v_Flags.x){"
    "case 0u:"
      "FragColor=t();"
      "break;"
    "case 1u:"
      "FragColor=t(u_BloomTexture2,0);"
      "break;"
    "case 2u:"
      "FragColor=t(u_BloomTexture2,1);"
      "break;"
    "case 3u:"
    "case 4u:"
      "vec3 v=texture(u_Texture,v_TexCoord).xyz;"
      "if(v_Flags.x==4u)"
        "{"
          "vec3 u=texture(u_BloomTexture1,v_TexCoord).xyz;"
          "u=vec3(1)-exp(-u*u_BloomExp);"
          "u=pow(u,vec3(1./u_BloomGamma));"
          "v=clamp(v+u,0.,1.);"
        "}"
      "FragColor=vec4(v,1);"
      "FragColor=v_TexIds.x>0?"
        "t(FragColor):"
        "FragColor;"
      "break;"
  "}"
  "FragColorMap=vec4(0);"
  "FragColorMask=vec4(0);"
"}"
"\n#elif COMPUTE\n"
"layout(local_size_x=16,local_size_y=16)in;"
"uniform sampler2D u_InTexture;"
"writeonly uniform image2D u_OutTexture;"
"uniform uint u_Flag=0;"
"vec4 v(sampler2D v,ivec2 u,int i)"
"{"
  "vec4 e=vec4(0);"
  "ivec2 F=textureSize(v,0)-1,f[17];"
  "if(i==0)"
    "for(int r=-8;r<=8;r++)"
      "f[r+8]=ivec2(clamp(u.x+r,1,F.x),u.y);"
  "else "
     "for(int r=-8;r<=8;r++)"
      "f[r+8]=ivec2(u.x,clamp(u.y+r,1,F.y));"
  "e+=texelFetch(v,f[0],0)*.0000777505714898;"
  "e+=texelFetch(v,f[1],0)*.0004886523773991;"
  "e+=texelFetch(v,f[2],0)*.002402783593204;"
  "e+=texelFetch(v,f[3],0)*.0092448129378371;"
  "e+=texelFetch(v,f[4],0)*.0278352765221734;"
  "e+=texelFetch(v,f[5],0)*.0655921303317756;"
  "e+=texelFetch(v,f[6],0)*.120980030195482;"
  "e+=texelFetch(v,f[7],0)*.174670181278773;"
  "e+=texelFetch(v,f[8],0)*.197416764383733;"
  "e+=texelFetch(v,f[9],0)*.174670181278773;"
  "e+=texelFetch(v,f[10],0)*.120980030195482;"
  "e+=texelFetch(v,f[11],0)*.0655921303317756;"
  "e+=texelFetch(v,f[12],0)*.0278352765221734;"
  "e+=texelFetch(v,f[13],0)*.0092448129378371;"
  "e+=texelFetch(v,f[14],0)*.002402783593204;"
  "e+=texelFetch(v,f[15],0)*.0004886523773991;"
  "e+=texelFetch(v,f[16],0)*.0000777505714898;"
  "return e;"
"}"
"void main()"
"{"
  "ivec2 u=ivec2(gl_GlobalInvocationID.xy);"
  "vec4 F=vec4(0);"
  "switch(u_Flag){"
    "case 0u:"
      "F=v(u_InTexture,u,0);"
      "break;"
    "case 1u:"
      "F=v(u_InTexture,u,1);"
      "break;"
  "}"
  "imageStore(u_OutTexture,u,F);"
"}"
"\n#endif"