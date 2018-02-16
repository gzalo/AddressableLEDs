#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <cmath>
#include <SDL2/SDL.h>
using namespace std;

float fase;
float bpm = 60.0f;
int beat = 0;
bool isBeat=false;

#if 0
	//OSC Support, not yet implemented
	const int PORT_NUM = 9001;
	float oscProg;
	int oscMap;
	ofstream out("log.txt");
	#define OSCPKT_OSTREAM_OUTPUT
	#include "oscpkt/oscpkt.hh"
	#include "oscpkt/udp.hh"
	using namespace oscpkt;
#endif

#include "serialPort.h"

unsigned char tabla[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255};

typedef struct rgb_color{
	uint8_t r,g,b;
} rgb_color;

#define LED_COUNT 120
rgb_color leds[LED_COUNT];

int trigAU8(float valor){
	return (int)round(127.5f*(valor + 1));
}

void tickOndaColor(float tick){
	for(int i=0;i<LED_COUNT;i++){
		float fase = 2*M_PI/LED_COUNT * (float)(i + tick*10.0) *5.0; 
		leds[i].r = trigAU8(sin(-fase));
		leds[i].g = trigAU8(sin(-fase*2.0/3.0 + M_PI/3.0));
		leds[i].b = trigAU8(sin(-fase*4.0/5.0 + M_PI*2.0/3.0))*0.1;
	}
}

void tickAguaCanioMedio(float ftick, rgb_color color){
	int tick = fmod(ftick,4.0)*LED_COUNT;
	
	tick /= 2;
	if((tick/4)%10 > 5){
		leds[LED_COUNT/2] = color;
	}else{
		leds[LED_COUNT/2] = (rgb_color){0,0,0};
	}
	
	for(int i=0;i<LED_COUNT/2;i++){
		leds[i] = leds [i+1];
	}
	for(int i=LED_COUNT-1;i>LED_COUNT/2;i--){
		leds[i] = leds [i-1];
	}
}

void tickAguaCanio(int tick, rgb_color color){
	if(rand()%100 == 0){
		leds[LED_COUNT-1] = color;
	}else{
		leds[LED_COUNT-1] = (rgb_color){0,0,0};
	}
	
	for(int i=0;i<LED_COUNT-1;i++){
		leds[i] = leds [i+1];
	}
}

rgb_color h2rgb(int h){
	if(h>360) h%=360;
	
	int r=0,g=0,b=0;
	int c=(h/60)%6+1;
	int crece=(h%60)*255.0/60.0;
	int decrece=(60.0-(h%60))*255.0/60.0;
	if(c==1)g=crece;
	if(c==2)r=decrece;
	if(c==3)b=crece;
	if(c==4)g=decrece;
	if(c==5)r=crece;
	if(c==6)b=decrece;
	if(c==1||c==6)r=255;
	if(c==2||c==3)g=255;
	if(c==4||c==5)b=255;
	return (rgb_color){(uint8_t)r,(uint8_t)g,(uint8_t)b};
}
void tickSpark(int tick, rgb_color color){
	for(int i=0;i<LED_COUNT;i++){
		leds[i].r *= 0.99;
		leds[i].g *= 0.99;
		leds[i].b *= 0.99;
	}
	if(rand()%5 == 0){
		int pos = rand()%LED_COUNT;
		leds[pos] = color;
	}
}

void tickHSV(float tick){
	float rtick = fmod(tick,2.0)/2.0;
	int i = rtick*LED_COUNT;
	leds[i%LED_COUNT] = h2rgb(tick*150);
}
void tickRand(float tick){
	if(rand()%10 == 0)
	for(int i=0;i<LED_COUNT;i++){
		leds[i].r = tabla[rand()%256];
		leds[i].g = tabla[rand()%256];
		leds[i].b = tabla[rand()%256];
	}
}
void tickRandUno(float tick){
	int idx = rand()%LED_COUNT;
		leds[idx].r = rand()%256;
		leds[idx].g = rand()%256;
		leds[idx].b = rand()%256;
	
	#if 0
		/* Variation of the effect, every 4 beats turn off all the LEDs */
		tick = fmod(tick, 4);
		if(tick < 0.1){
			for(int i=0;i<LED_COUNT;i++){
				leds[i].r = 0;
				leds[i].g = 0;
				leds[i].b = 0;
			}	
		}
	#endif
}
void tickRandMarch(float tick, bool puros = false){
	
	static int dir = -1;
	
	/*if(rand()%50 == 0){
		dir *= -1;
	}*/
	tick = fmod(tick,1.0);
	
	if(rand()%20==0){
		for(int i=0;i<LED_COUNT;i++){
			if(puros){
				int v = rand()%3;
				
				switch(v){
					case 0:
						leds[i].r = 255, leds[i].g = 0, leds[i].b = 0;
						break;						
					case 1:
						leds[i].r = 0, leds[i].g = 255, leds[i].b = 0;
						break;						
					case 2:
						leds[i].r = 0, leds[i].g = 0, leds[i].b = 255;
						break;							
				}
				
			}else{
				leds[i].r = rand()%256;
				leds[i].g = rand()%256;
				leds[i].b = rand()%256;
			}
		}
	}else{
		if(isBeat){
			isBeat = false;
			if(dir == 1){
				rgb_color l0 = leds[0];
				for(int i=0;i<LED_COUNT-1;i++){
					leds[i].r = leds[i+1].r;
					leds[i].g = leds[i+1].g;
					leds[i].b = leds[i+1].b;
				}
				leds[LED_COUNT-1] = l0;
			}else{
				rgb_color lf = leds[LED_COUNT-1];
				for(int i=LED_COUNT-1;i>0;i--){
					leds[i].r = leds[i-1].r;
					leds[i].g = leds[i-1].g;
					leds[i].b = leds[i-1].b;
				}
				leds[0] = lf;
			}
		}
	}
}

void tickRandMarch2(float tick, bool puros, int cambiar){
	if(!cambiar) return;
	
	for(int i=0;i<LED_COUNT;i++){
				leds[i].r = rand()%256;
				leds[i].g = rand()%256;
				leds[i].b = rand()%256;
	}
}


void tickGauss(float tick, rgb_color color){
	///TODO: FIX!
	tick = fmod(tick,4.0);
	const float ancho = 10;
	
	float tick2 = tick*LED_COUNT*0.25;
	
	for(int i=0;i<LED_COUNT;i++){
		float normal = 0.0f;
		for(int j=-8;j<8;j++){
			normal += exp(-(float)(i-tick2+LED_COUNT*0.25*j)*(float)(i-tick2+LED_COUNT*0.25*j)/ancho);
		}
		normal /= 2.0f;
		leds[i].r = color.r * normal;
		leds[i].g = color.g * normal;
		leds[i].b = color.b * normal;
	}
}

void tickPingPong(float tick, rgb_color color){
	for(int i=0;i<LED_COUNT;i++){
		leds[i].r = 0;
		leds[i].g = 0;
		leds[i].b = 0;
	}
	
	int idx = round((float)LED_COUNT*0.5f*(sin(tick*2.0f*M_PI/4.0f)+1))-1;
	leds[idx%LED_COUNT] = color;
}

void tickPingPongLineal(float tick, rgb_color color){
	for(int i=0;i<LED_COUNT;i++){
		leds[i].r = 0;
		leds[i].g = 0;
		leds[i].b = 0;
	}
	tick = fmod(tick,4.0f);
	int cambio = tick*0.25*LED_COUNT;
	leds[cambio%LED_COUNT] = color;
}

void tickPingPongLleno(float tick, rgb_color color0, rgb_color color1){
	
	int idx = round((float)LED_COUNT*0.5f*(sin(tick*2.0*M_PI/4.0)+1));
	
	for(int i=0;i<LED_COUNT;i++){
		if(i<idx) leds[i] = color0; else leds[i] = color1;
	}
}

const unsigned char tablaRad [128] = {
	69,74,80,86,92,98,105,112,119,126,134,142,149,157,165,172,180,188,195,202,209,216,222,228,233,238,243,246,249,252,254,255,255,255,254,252,249,246,243,238,233,228,222,216,209,202,195,188,180,172,165,157,149,142,134,126,119,112,105,98,92,86,80,74,69,63,58,54,49,45,41,38,34,31,28,25,22,20,18,16,14,12,10,9,7,6,5,4,3,2,2,1,1,0,0,0,0,0,0,0,1,1,2,2,3,4,5,6,7,9,10,12,14,16,18,20,22,25,28,31,34,38,41,45,49,54,58,63
};

void tickRadiacion(float tick, int efe){
	tick = fmod(tick, 1.0f);
	
	for(int i=0;i<LED_COUNT;i++){
		int v = (i/4)%2;
		if(v == 0 && efe/2 == 0){
			float n = fmod(tick, 1);
			
			leds[i].r = rand()%16;
			leds[i].g = tablaRad[(int)(128.0f*n) % 128];
			leds[i].b = rand()%16;
		}else if(v == 1 && efe/2 == 1){
			float n = fmod(tick, 1);
			
			leds[i].r = tablaRad[(int)(128.0f*n) % 128];
			leds[i].g = 0;
			leds[i].b = 0;
		}
	}
}	

void actualizarLeds(){
	unsigned char buffer[1+LED_COUNT*3] = {0xFF};
	for(int j=0;j<LED_COUNT;j++){
		buffer[j*3+1] = (leds[j].r==0xFF)?254:leds[j].r;	
		buffer[j*3+2] = (leds[j].g==0xFF)?254:leds[j].g;
		buffer[j*3+3] = (leds[j].b==0xFF)?254:leds[j].b;
    
		//Lower brightness alternative
/*    buffer[j*3+1] = leds[j].r/4;	
		buffer[j*3+2] = leds[j].g/4;
		buffer[j*3+3] = leds[j].b/4;*/
	}
	
	int dwBytesSent = serialWrite(buffer, 1+LED_COUNT*3);
  
	if(dwBytesSent != 1+LED_COUNT*3){
		cerr << "Can't write to serial port." << endl;
	}
}

int tBeats[4];

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

const int anchoPantalla = 120*8;
const int altoPantalla = 8*2;

int videoInit(){
	
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
		printf("SDL Init Error: %s\n", SDL_GetError());
		return 1;
	}
	
	gWindow = SDL_CreateWindow("AddressableLEDs", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, anchoPantalla, altoPantalla, SDL_WINDOW_SHOWN);
	if(!gWindow){
		printf("SDL Window Error: %s\n", SDL_GetError());
		return 1;
	}
		
	gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
	if(gRenderer == NULL ){
		printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
		return 1;
	}
	return 0;
}

int videoEnd(){
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	SDL_Quit();
	return 0;
}

void renderLeds(){
  float maxInt = 0;
	for(int i=0;i<LED_COUNT;i++){
		SDL_SetRenderDrawColor(gRenderer, leds[i].r, leds[i].g, leds[i].b, 255);
		SDL_Rect rectangle;

		rectangle.x = i*8;
		rectangle.y = 0;
		rectangle.w = 8;
		rectangle.h = 8;
		SDL_RenderFillRect(gRenderer, &rectangle);
		
    float intensity = sqrt(leds[i].r*leds[i].r + leds[i].g*leds[i].g+leds[i].b*leds[i].b);
    
    if(intensity > maxInt)
      maxInt = intensity;
	}
  
  float compFactor = 255/maxInt;
  
  for(int i=0;i<LED_COUNT;i++){
		SDL_SetRenderDrawColor(gRenderer, leds[i].r*compFactor, leds[i].g*compFactor, leds[i].b*compFactor, 255);
		SDL_Rect rectangle;

		rectangle.x = i*8;
		rectangle.y = 8;
		rectangle.w = 8;
		rectangle.h = 8;
		SDL_RenderFillRect(gRenderer, &rectangle);
		
	}
}


int main(int argc, char **args){
	int port = serialInit();
	if(port != -1)
		cout << "Opened port " << port << endl;
	
	#if 0
		UdpSocket sock; 
		
		sock.bindTo(PORT_NUM);
		
		if(!sock.isOk()) {
			cerr << "Error opening port " << PORT_NUM << ": " << sock.errorMessage() << "\n";
			return -1;
		}
	#endif
	int oldTicks = 0;
	int ledTicks = 0;
	int efecto = 0;
	
	#if 0
	//bool sonando = false;
	//int oldBeatTick = SDL_GetTicks();
	//int cantBeats = 0;
	//float prom = 0.0f;
	PacketReader pr;
	while (sock.isOk()) {      
		if (sock.receiveNextPacket(1 /* timeout, in ms */)) {
			pr.init(sock.packetData(), sock.packetSize());
			oscpkt::Message *msg;
			while (pr.isOk() && (msg = pr.popMessage()) != 0) {
				int iarg[2];
				float farg[4];
				if (msg->match("/live/beat").popInt32(iarg[0]).isOkNoMoreArgs()) {
					cout << "Beat " << beat << endl;	
					beat++;
					
					tBeats[0] = tBeats[1];
					tBeats[1] = tBeats[2];
					tBeats[2] = tBeats[3];
					tBeats[3] = SDL_GetTicks();
					
					float estimT = -0.3*tBeats[0]-0.1*tBeats[1]+0.1*tBeats[2]+0.3*tBeats[3];
					float estimP =  0.7*tBeats[0]+0.4*tBeats[1]+0.1*tBeats[2]-0.2*tBeats[3];
					
					//bpm = 60.0f * 1000.0f / estimT;
					printf("BPM: %f\n", bpm);
					
					estimP += 5*estimT;
					printf("P0: %f, %f\n", estimP, (float)tBeats[3]);
					
					//fase = 0.9*fase+0.1*estimP/estimT;
					printf("Fase: %f", fase);
					
					isBeat = true;
					//fase = 0;
					
					/*Enganche random: */if(rand()%10 == 0)fase = round(fase);
					/*float delta = SDL_GetTicks() - oldBeatTick; //ms hasta el beat anterior
					bpm = 60.0f * 1000.0f / delta;*/
					
					/*
					Promedio de muestras de BPM
					delta = 60.0f * 1000.0f / delta;
					prom += delta;
					cantBeats++;*/
					//cout << prom / (float)cantBeats << endl;
					
					/*
					Randomizar LEDs
					for(int i=0;i<LED_COUNT;i++){
						leds[i].r = rand()%256;
						leds[i].g = rand()%256;
						leds[i].b = rand()%256;
					}*/
					
					oldBeatTick = SDL_GetTicks();
					out << SDL_GetTicks() << endl;
				}else if (msg->match("/live/play").popInt32(iarg[0]).isOkNoMoreArgs()) {
					if(iarg[0] == 1)
						sonando = false;
					else	
						sonando = true;
				}else if(msg->match("/live/clip/position").
						popInt32(iarg[0]).popInt32(iarg[1]).
						popFloat(farg[0]).popFloat(farg[1]).popFloat(farg[2]).popFloat(farg[3]).isOkNoMoreArgs()){
					
					oscProg = farg[0]; //En beats
					iarg[1]++;
					
					if(oscMap != iarg[1]){
						//Cambio de pista
						oscMap = iarg[1];
						cout << "Pista " << oscMap << endl;
					}

				}else{
					//cout << *msg << "\n";
				}
			}
		}
	#endif
	
	videoInit();
	
	bool quit = false;
	while(!quit){	
		
		SDL_Event event;
		while(SDL_PollEvent(&event)){
			if(event.type == SDL_QUIT) quit = true;
			if(event.type == SDL_KEYDOWN){
				if(event.key.keysym.sym == SDLK_ESCAPE)
					quit = true;
			}
		}
		
		if(SDL_GetTicks() - oldTicks > 16){
			float delta = SDL_GetTicks() - oldTicks;
			float periodo = 1.0f/(bpm / 60.0f);
			fase += ((delta/1000.0f) / periodo);  
			
			#define NUMEFECTOS 8
			efecto = (int)(fase/8.0f)%NUMEFECTOS;
			
			//Christmas themed effects
			switch(efecto){
			case 0: tickRadiacion(fase/2.0,2); break;
			case 1:tickPingPongLleno(fase/2.0, (rgb_color){32,0,0}, (rgb_color){32,32,16});break;
			case 2:tickGauss(fase/2.0, (rgb_color){32,16,0});break;
			case 3:tickRandMarch2(fase, true, ledTicks%16==0);break;
			case 4:tickHSV(fase);break;
			case 5:tickSpark(fase, (rgb_color){32,16,0});break;
			case 6:tickAguaCanio(fase/2.0, (rgb_color){16,16,0});break;
			case 7:tickOndaColor(fase/2.0); break;
			}
			
			//Old effect list
			/*
			
			switch(efecto){
				case 0: tickRadiacion(fase,2); break;
				case 1: tickPingPongLleno(fase, (rgb_color){32,0,0}, (rgb_color){00,0,32}); break;
				case 2: tickPingPong(fase, (rgb_color){255,255,255}); break;
				case 3: tickPingPongLineal(fase, (rgb_color){32,32,32}); break;
				case 4: tickGauss(fase, (rgb_color){255,128,0}); break;
				case 5: tickRandMarch(fase, false); break;
				case 6: tickRandMarch(fase, true); break;
				case 7: tickRandUno(fase); break;
				case 8: tickRand(fase); break;
				case 9: tickHSV(fase); break;
				case 10: tickSpark(fase, (rgb_color){255,255,0}); break;
				case 11: tickAguaCanio(fase, (rgb_color){16,16,16}); break;
				case 12: tickAguaCanioMedio(fase, (rgb_color){rand()%64,0,0}); break;
				case 13: tickOndaColor(fase); break;
				default: break;
			}*/

			#if 0
			if(!sonando){
				for(int i=0;i<LED_COUNT;i++)
					leds[i].r = leds[i].g = leds[i].b = 0;
			}
			#endif

			SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00 );
			SDL_RenderClear(gRenderer);		
	
			#if 0
			//Random LED test
			for(int i=0;i<LED_COUNT;i++)
				leds[i].r = leds[i].g = leds[i].b = rand()%250 + 1;
			#endif
	
	
			actualizarLeds();
			renderLeds();
  
			SDL_Delay(20);
	
			SDL_RenderPresent(gRenderer);
			
			ledTicks++;
			oldTicks = SDL_GetTicks();
		}
	} 
	serialClose();
	videoEnd();
	return 0;
}
