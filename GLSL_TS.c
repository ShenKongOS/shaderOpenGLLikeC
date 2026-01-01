#define Animation
//#define DOS
#define WINDOWS
//#define UNIX

#define resolutionX 35
#define resolutionY 35

#include<stdio.h>
#include<string.h> // memset需要
#define _USE_MATH_DEFINES  // 启用math.h中的M_PI
#include<math.h>
#ifdef Animation
//获取时间
#include<windows.h>
#endif
#ifdef DOS
#include <dos.h>
#endif

typedef struct RESOLUTION{
	int x;
	int y;
} RESOLUTION; 

typedef struct FRAGCOORD{
	int x;
	int y;
} FRAGCOORD; 

typedef struct vec2{
	float x;
	float y;
} vec2; 

typedef struct vec3{
	float x;
	float y;
	float z;
} vec3; 

typedef struct vec4{
	float x;
	float y;
	float z;
	float w;
} vec4; 


// 根据R(x)/G(y)/B(z)计算ANSI前景色码
int get_ansi_color(float r, float g, float b);


float step(float value,float i);
vec2 step2( float value,vec2 i);
float smoothstep(float a1,float a2,float x);
float length(vec2 st);

// 二维向量点乘
float dot2(vec2 a, vec2 b);
// 三维向量点乘
float dot3(vec3 a, vec3 b) ;
// 四维向量点乘
float dot4(vec4 a, vec4 b) ;

#ifdef Animation
// 获取高精度时间
double get_time_us(void);

#endif

//输出显示
void show(float FrameBuffer[][resolutionX][4],RESOLUTION u_resolution);
//预处理颜色(动态坐标)
void preprocessingColorST(vec2 * st);

vec4 frag_GL(FRAGCOORD cFragCoord,RESOLUTION u_resolution,double time);

//写入帧缓冲
void FrameBufferWrite(vec4 color,float FrameBuffer[][resolutionX][4],FRAGCOORD coord);


#ifdef Animation

//时间
double time_start;
double time;
#else
double time=1.;
#endif
//默认缓冲
float FrameBuffer[resolutionY][resolutionX][4]={0};
// 字符双缓冲核心：后台字符串缓冲区（每行最多16字符/像素 + 换行，留冗余）
#define BUF_SIZE ((resolutionX * 16 + 2) * resolutionY + 1)
char BackBuffer[BUF_SIZE] = {0}; // 后台缓冲：存储整帧输出内容


int main(){
	//分辨率
	RESOLUTION u_resolution;
	u_resolution.x=resolutionX;
	u_resolution.y=resolutionY;
	//输出颜色
	vec4 outColor;
	//对应像素位置坐标
	FRAGCOORD cFragCoord = {0,0};
#ifdef Animation
	time_start = get_time_us();
	
	
	while(1){
		//time =  get_time_us()-time_start;
		time = (get_time_us() - time_start) / 1000000.0;
#endif
		//绘制一帧
		for(cFragCoord.y=0;cFragCoord.y<u_resolution.y;cFragCoord.y++){
			for(cFragCoord.x=0;cFragCoord.x<u_resolution.x;cFragCoord.x++){
				
				
				outColor = frag_GL(cFragCoord,u_resolution,time);
				
				
				FrameBufferWrite(outColor,FrameBuffer,cFragCoord);
				
			}
			
		}
#ifdef Animation
		
		show(FrameBuffer,u_resolution);
		Sleep(16);
		printf("\033[2J\033[H"); // ANSI清屏+光标归位，比system("cls")快
		
	}
#else
	
	show(FrameBuffer,u_resolution);
#endif
	return 0;
}

// 根据R(x)/G(y)/B(z)计算ANSI前景色码
int get_ansi_color(float r, float g, float b) {
	int base = 30; // 基础色起始码（30-37）
	// 映射R/G/B到颜色位（0~1 → 0/1）
	// 多级阈值，让颜色过渡更细腻
	int r_bit = (r >= 0.3) ? 1 : 0;
	int g_bit = (g >= 0.3) ? 1 : 0;
	int b_bit = (b >= 0.3) ? 1 : 0;
	// 计算基础颜色码（30=黑,31=红,32=绿,33=黄,34=蓝,35=洋红,36=青,37=白）
	int color_code = base + (r_bit * 1) + (g_bit * 2) + (b_bit * 4);
	// 用z分量控制亮度（z>0.5则切换到亮色系90-97）
	if (b > 0.5) { // 这里用z（b）控制亮度，也可单独加亮度判断
		color_code += 60; // 30+60=90（亮黑），31+60=91（亮红）...
	}
	return color_code;
}


float step(float value,float i){
	if(i>value){
		return 1.0;	
	}
	else{
		return 0;
	}
}

vec2 step2( float value,vec2 i){
	vec2 returnValue;
	
	if(i.x>value){
		returnValue.x= 1.0;	
	}
	else{
		returnValue.x =0;
	}
	
	if(i.y>value){
		returnValue.y= 1.0;	
	}
	else{
		returnValue.y= 0;
	}
	
	return returnValue;
}

// 二维向量点乘
float dot2(vec2 a, vec2 b) {
	return a.x * b.x + a.y * b.y;
}

// 三维向量点乘
float dot3(vec3 a, vec3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// 四维向量点乘
float dot4(vec4 a, vec4 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float length(vec2 st){
	return sqrt(dot2(st,st));
}

float smoothstep(float a1,float a2,float x){
	float u,t;
	
	t = (x - a1) / (a2 - a1);
	
	
	u= t * t * (3.0 - 2.0 * t);
	//等同于t = clamp(t, 0.0, 1.0);
	if(t>1.){
		return 1.;
	}
	else if(t<0.){
		return 0.;
	}
	else{
		
		return u;
	}
	
}



#ifdef Animation
// 获取高精度时间
double get_time_us(void) {
	LARGE_INTEGER freq, cnt;
	QueryPerformanceFrequency(&freq); // 获取CPU频率
	QueryPerformanceCounter(&cnt);    // 获取当前计数
	return (double)cnt.QuadPart / freq.QuadPart * 1000000.0; // 转换为微秒
}
#endif

#ifdef WINDOWS
//输出显示缓冲
void show(float FrameBuffer[][resolutionX][4],RESOLUTION u_resolution){
	FRAGCOORD cFragCoord = {0,0};
	
	//从默认到字符缓冲
	// 重置后台缓冲区（每次绘制前清空）
	memset(BackBuffer, 0, BUF_SIZE);
	// 缓冲区写入位置指针
	int buf_pos = 0;
	
	for(cFragCoord.y=u_resolution.y-1;cFragCoord.y>=0;cFragCoord.y--){
		for(cFragCoord.x=0;cFragCoord.x<u_resolution.x;cFragCoord.x++){
			int va = get_ansi_color(FrameBuffer[cFragCoord.y][cFragCoord.x][0],
									FrameBuffer[cFragCoord.y][cFragCoord.x][1],
									FrameBuffer[cFragCoord.y][cFragCoord.x][2]);
			// 将ANSI颜色码和字符写入后台缓冲区（替代printf）
			buf_pos += snprintf(BackBuffer + buf_pos, BUF_SIZE - buf_pos, 
								"\033[%dm|o|\033[0m", va);
		}
		// 每行结束添加换行符
		buf_pos += snprintf(BackBuffer + buf_pos, BUF_SIZE - buf_pos, "\n");
	}
	
	
	//一次性输出后台缓冲区（前台显示，无闪烁）
	printf("%s", BackBuffer);
	
//	for(cFragCoord.y=u_resolution.y-1;cFragCoord.y>=0;cFragCoord.y--){
//		for(cFragCoord.x=0;cFragCoord.x<u_resolution.x;cFragCoord.x++){
//			
//			//printf("%.2f ",FB[cFragCoord.x*4+u_resolution.x*cFragCoord.y*4+i]);
//			int va = get_ansi_color(FrameBuffer[cFragCoord.y][cFragCoord.x][0],
//									FrameBuffer[cFragCoord.y][cFragCoord.x][1],
//									FrameBuffer[cFragCoord.y][cFragCoord.x][2]);
//			printf("\033[%dm|o|\033[0m", va);
//			
//		}
//		printf("\n");
//	}
}
#endif

//预处理颜色(动态坐标)
void preprocessingColorST(vec2 * st){
	(*st).x = fabs((*st).x);
	(*st).y = fabs((*st).y);
}

vec4 frag_GL(FRAGCOORD cFragCoord,RESOLUTION resolution,double time){
	vec4 color;
	vec2 st = {(float)cFragCoord.x/resolution.x,(float)cFragCoord.y/resolution.y};
	
	st.x-=0.5;
	st.y-=0.5;
	st.x*=2.;
	st.y*=2.;
	
	
//				float a = atan2f(st.y,st.x)+M_PI*0.16;
//				float part = (M_PI*2.)/3.;
//				float r = cos(floor(0.5+a/part)*part-a)*length(st);
//				float d = 1. - smoothstep(0.39,0.31,r);
//	
	float a = atan2f(st.y,st.x);
	float r = length(st)*2.;
	float f = cos(a*3.+time*M_PI);
	float d =1.- smoothstep(f,f+0.01,r);
//	float f =dot2(st,st);
//	float d =1.- smoothstep(0.3,0.31,f);
	
	//处理负值
	preprocessingColorST(&st);
	//输出颜色
	color = (vec4) {d,d,0.1,1.0};
	return color;
}

//写入帧缓冲
void FrameBufferWrite(vec4 color,float  FrameBuffer[][resolutionX][4],FRAGCOORD coord){
	
	
	FrameBuffer[coord.y][coord.x][0]=color.x;
	FrameBuffer[coord.y][coord.x][1]=color.y;
	FrameBuffer[coord.y][coord.x][2]=color.z;
	FrameBuffer[coord.y][coord.x][3]=color.w;
	

}

