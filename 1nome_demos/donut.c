/**
  ******************************************************************************
  * @file     example_08-donut.c
  * @author   Adam Wiszczor, a1k0n
  * @brief    Vykresluje donut pres UART, treba nastavit vyssi frekvenci jadra i uartu pro spravny efekt
  *
  ******************************************************************************
  * @attention
  *
  * Otestovano na: F407
  *
  * Netestovano: F401, F411, G071, L152
  *
  ******************************************************************************
*/

#include "stm32_kit.h"
#include "stm32_kit/uart.h"
#include "stm32_kit/overclock.h"
#include <string.h>
#include <stdio.h>

BOARD_SETUP void setup(void) {
	//clock168Mhz();
	UART_setup();
};

int k;
float z[1760];
char b[1760];
float sini,
      cosj,
      sinA,
      sinj,
      cosA,
      cosj2,
      cosi,
      cosB,
      sinB,
      t, mess;
int x, y, o, N, count = 0;
unsigned int i, j, A=0, B=0;
char counter[10];


float lut[628] = {0,0.00999983,0.0199987,0.0299955,0.0399893,0.0499792,0.059964,0.0699428,0.0799147,0.0898785,0.0998334,0.109778,0.119712,0.129634,0.139543,0.149438,0.159318,0.169182,0.17903,0.188859,0.198669,0.20846,0.21823,0.227978,0.237703,0.247404,0.257081,0.266731,0.276356,0.285952,0.29552,0.305059,0.314567,0.324043,0.333487,0.342898,0.352274,0.361615,0.37092,0.380188,0.389418,0.398609,0.40776,0.416871,0.425939,0.434966,0.443948,0.452886,0.461779,0.470626,0.479426,0.488177,0.49688,0.505533,0.514136,0.522687,0.531186,0.539632,0.548024,0.556361,0.564642,0.572867,0.581035,0.589145,0.597195,0.605186,0.613117,0.620986,0.628793,0.636537,0.644218,0.651834,0.659385,0.66687,0.674288,0.681639,0.688921,0.696135,0.703279,0.710353,0.717356,0.724287,0.731146,0.737931,0.744643,0.75128,0.757843,0.764329,0.770739,0.777072,0.783327,0.789504,0.795602,0.80162,0.807558,0.813416,0.819192,0.824886,0.830497,0.836026,0.841471,0.846832,0.852108,0.857299,0.862404,0.867423,0.872355,0.877201,0.881958,0.886627,0.891207,0.895699,0.9001,0.904412,0.908633,0.912764,0.916803,0.920751,0.924606,0.928369,0.932039,0.935616,0.939099,0.942489,0.945784,0.948985,0.95209,0.955101,0.958016,0.960835,0.963558,0.966185,0.968715,0.971148,0.973485,0.975723,0.977865,0.979908,0.981854,0.983701,0.98545,0.9871,0.988652,0.990105,0.991458,0.992713,0.993868,0.994924,0.995881,0.996738,0.997495,0.998152,0.99871,0.999168,0.999526,0.999784,0.999942,1,0.999958,0.999816,0.999574,0.999232,0.99879,0.998248,0.997606,0.996865,0.996024,0.995083,0.994043,0.992904,0.991665,0.990327,0.98889,0.987354,0.985719,0.983986,0.982154,0.980224,0.978197,0.976071,0.973848,0.971527,0.969109,0.966594,0.963983,0.961275,0.958471,0.955572,0.952576,0.949486,0.9463,0.94302,0.939645,0.936177,0.932615,0.92896,0.925212,0.921371,0.917438,0.913413,0.909297,0.905091,0.900793,0.896406,0.891929,0.887362,0.882707,0.877964,0.873133,0.868215,0.863209,0.858118,0.85294,0.847678,0.84233,0.836899,0.831383,0.825785,0.820104,0.814341,0.808496,0.802571,0.796565,0.79048,0.784316,0.778073,0.771753,0.765355,0.758881,0.752331,0.745705,0.739005,0.732231,0.725384,0.718465,0.711473,0.704411,0.697278,0.690075,0.682803,0.675463,0.668056,0.660581,0.653041,0.645435,0.637765,0.630031,0.622234,0.614374,0.606454,0.598472,0.590431,0.582331,0.574172,0.565956,0.557684,0.549355,0.540972,0.532535,0.524044,0.515501,0.506907,0.498262,0.489567,0.480823,0.472031,0.463191,0.454306,0.445375,0.436399,0.42738,0.418318,0.409214,0.400069,0.390885,0.381661,0.372399,0.3631,0.353764,0.344393,0.334988,0.325549,0.316078,0.306575,0.297041,0.287478,0.277886,0.268266,0.258619,0.248947,0.239249,0.229528,0.219784,0.210017,0.20023,0.190423,0.180596,0.170752,0.16089,0.151013,0.14112,0.131213,0.121293,0.111361,0.101418,0.0914646,0.0815022,0.0715315,0.0615537,0.0515698,0.0415807,0.0315874,0.021591,0.0115924,0.00159265,-0.00840725,-0.0184063,-0.0284035,-0.0383979,-0.0483884,-0.0583741,-0.068354,-0.078327,-0.0882922,-0.0982486,-0.108195,-0.118131,-0.128055,-0.137966,-0.147863,-0.157746,-0.167612,-0.177462,-0.187295,-0.197108,-0.206902,-0.216675,-0.226427,-0.236155,-0.245861,-0.255541,-0.265196,-0.274825,-0.284426,-0.293998,-0.303542,-0.313054,-0.322536,-0.331985,-0.341401,-0.350783,-0.36013,-0.369441,-0.378715,-0.387951,-0.397148,-0.406306,-0.415423,-0.424498,-0.433531,-0.44252,-0.451466,-0.460366,-0.46922,-0.478027,-0.486787,-0.495497,-0.504159,-0.512769,-0.521329,-0.529836,-0.538291,-0.546691,-0.555037,-0.563327,-0.571561,-0.579738,-0.587857,-0.595917,-0.603918,-0.611858,-0.619737,-0.627554,-0.635308,-0.642999,-0.650625,-0.658186,-0.665682,-0.673111,-0.680473,-0.687766,-0.694991,-0.702146,-0.709231,-0.716246,-0.723188,-0.730058,-0.736856,-0.743579,-0.750228,-0.756802,-0.763301,-0.769723,-0.776068,-0.782336,-0.788525,-0.794636,-0.800667,-0.806618,-0.812488,-0.818277,-0.823984,-0.829609,-0.835151,-0.840609,-0.845984,-0.851273,-0.856478,-0.861597,-0.86663,-0.871576,-0.876435,-0.881206,-0.885889,-0.890484,-0.894989,-0.899405,-0.903732,-0.907967,-0.912112,-0.916166,-0.920128,-0.923998,-0.927776,-0.931461,-0.935053,-0.938551,-0.941955,-0.945266,-0.948481,-0.951602,-0.954628,-0.957558,-0.960392,-0.963131,-0.965773,-0.968319,-0.970767,-0.973119,-0.975373,-0.97753,-0.979589,-0.98155,-0.983413,-0.985178,-0.986844,-0.988411,-0.98988,-0.991249,-0.99252,-0.993691,-0.994763,-0.995735,-0.996608,-0.997381,-0.998054,-0.998628,-0.999102,-0.999476,-0.999749,-0.999923,-0.999997,-0.999971,-0.999845,-0.999619,-0.999293,-0.998867,-0.998341,-0.997715,-0.99699,-0.996165,-0.99524,-0.994216,-0.993092,-0.991869,-0.990547,-0.989125,-0.987605,-0.985986,-0.984269,-0.982453,-0.980538,-0.978526,-0.976416,-0.974208,-0.971903,-0.969501,-0.967001,-0.964405,-0.961713,-0.958924,-0.95604,-0.95306,-0.949984,-0.946814,-0.943549,-0.940189,-0.936736,-0.933189,-0.929548,-0.925815,-0.921989,-0.91807,-0.91406,-0.909959,-0.905767,-0.901484,-0.897111,-0.892648,-0.888096,-0.883455,-0.878725,-0.873908,-0.869004,-0.864012,-0.858934,-0.853771,-0.848522,-0.843188,-0.837769,-0.832267,-0.826682,-0.821014,-0.815264,-0.809433,-0.80352,-0.797527,-0.791455,-0.785303,-0.779073,-0.772764,-0.766379,-0.759917,-0.753379,-0.746765,-0.740077,-0.733315,-0.72648,-0.719572,-0.712592,-0.70554,-0.698418,-0.691227,-0.683966,-0.676637,-0.66924,-0.661776,-0.654246,-0.646651,-0.638991,-0.631267,-0.62348,-0.61563,-0.607719,-0.599747,-0.591716,-0.583625,-0.575475,-0.567269,-0.559005,-0.550686,-0.542311,-0.533882,-0.5254,-0.516865,-0.508279,-0.499642,-0.490955,-0.482218,-0.473434,-0.464602,-0.455724,-0.4468,-0.437832,-0.428819,-0.419764,-0.410667,-0.401529,-0.39235,-0.383133,-0.373877,-0.364583,-0.355254,-0.345888,-0.336488,-0.327055,-0.317589,-0.308091,-0.298562,-0.289003,-0.279415,-0.2698,-0.260157,-0.250489,-0.240795,-0.231078,-0.221337,-0.211574,-0.20179,-0.191986,-0.182163,-0.172321,-0.162462,-0.152587,-0.142697,-0.132792,-0.122874,-0.112944,-0.103002,-0.0930505,-0.0830894,-0.07312,-0.0631433,-0.0531602,-0.0431719,-0.0331792,-0.0231832,-0.0131849,
};

float fastsin(unsigned int x)
{
  return lut[x%628];
}

float fastcos(unsigned int x){
	return fastsin(x + 157);
}

#define QualityI 7 // cim vetsi tim mensi kvalita
#define QualityJ 2
#define SpeedA 4
#define SpeedB 2

int main(){
	
	UART_write("\x1b[2J",4);
    for(; ; ) {
		snprintf(counter, 10, "%8d\n", count++);
		UART_write(counter, 9);
        memset(b,32,1760);
        memset(z,0,7040);
        for(j=0; 628>j; j+=QualityJ) {
            for(i=0; 628 >i; i+=QualityI) {
                sini=fastsin(i);
                cosj=fastcos(j);
                sinA=fastsin(A);
                sinj=fastsin(j);
                cosA=fastcos(A);
                cosj2=cosj+2;
                mess=1/(sini*cosj2*sinA+sinj*cosA+5);
                cosi=fastcos(i);
                cosB=fastcos(B);
                sinB=fastsin(B);
                t=sini*cosj2*cosA-sinj* sinA;
                x=40+30*mess*(cosi*cosj2*cosB-t*sinB);
                y= 12+15*mess*(cosi*cosj2*sinB +t*cosB);
                o=x+80*y;
                N=8*((sinj*sinA-sini*cosj*cosA)*cosB-sini*cosj*sinA-sinj*cosA-cosi *cosj*sinB);
                if(22>y&&y>0&&x>0&&80>x&&mess>z[o]){
                    z[o]=mess;
                    b[o]=".,-~:;=!*#$@"[N>0?N:0];
                }
            }
        }
		UART_write("\x1b[d",3);
        for(k=0; 1761>k; k++){
            UART_putc(k%80?b[k]:'\r');
			if(!(k%80)){
				UART_putc('\n');
			}
		}
        A+=SpeedA;
        B+=SpeedB;
    }
}
