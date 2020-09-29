#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>

enum type {MINING, RESEARCH, BOOSTER, DUPLICATOR, STORAGE, BASIC};
enum class { S, A, B, C, D, E, F };
enum level { NA, G1, G2, G3, G4, G5, G6, G7, G8, G9, G10 };

/*
 * production
 * A = 500, B = 350, C = 250
 * +70% --> += (class.value/100*70)
 *
 * revenue
 */

/* These are for 100%. Basic probe is 50% so half of these values. */
const int production_values[] = { 0, 500, 350, 250, 0,   0,   0   };
const int revenue_values[]    = { 0, 750, 650, 550, 450, 300, 200 };

/* value of each siteseeing spot for probe level: G1, G2, G3, G4, G5, G6 */
const int sightseeing_values[]					= { 0, 2000, 2500, 3000, 3500, 4000, 4500 };
const float research_probe_percentages[]			= { 0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0 };
const float mining_probe_percentages[]			= { 0, 0.5, 0.7, 0.9, 1.1, 1.3, 1.5, 1.7, 1.9, 2.2, 2.5 };

struct fn_site
{
	int connected[4];
	int production;
	int revenue;
	int combat;
	int sightseeing;
};

struct probe
{
	int type;
	int level;
	int number;
};

struct probe probes[] =
{
	{MINING, G1, 16},
	{MINING, G2, 16},
	{MINING, G3, 4},
	{MINING, G4, 13},
	{MINING, G5, 7},
	{MINING, G6, 7},
	{MINING, G7, 3},
	{MINING, G8, 14},
	{MINING, G9, 3},
	{MINING, G10, 2},
	{RESEARCH, G1, 3},
	{RESEARCH, G2, 3},
	{RESEARCH, G3, 1},
	{RESEARCH, G4, 3},
	{RESEARCH, G5, 5},
	{RESEARCH, G6, 3},
	{BASIC, NA, 104},
	//{BOOSTER, G1, 1},
	//{STORAGE, NA, 5},
	//{DUPLICATOR, NA, 1},
};

#define fn_total 104

const struct fn_site fn_sites[fn_total] =
{
	{{4, -1, -1, -1},			C,	A,	S,	1}, // 101
	{{3, -1, -1, -1},			C,	F,	B,	0}, // 102
	{{5, 42, 4, -1},			C,	E,	A,	1}, // 103
	{{5, 1, -1, -1},			C,	A,	B,	1}, // 104
	{{2, 8, 0, -1},				A,	F,	B,	0}, // 105
	{{6, 3, 2, -1},				B,	E,	B,	1}, // 106
	{{9, 5, -1, -1},			A,	F,	B,	0}, // 107
	{{8, -1, -1, -1},			C,	F,	B,	0}, // 108
	{{4, 7, -1, -1},			C,	D,	B,	0}, // 109
	{{6, 10, 11, -1},			C,	E,	B,	1}, // 110
	{{9, 12, -1, -1},			C,	F,	B,	0}, // 111
	{{9, 13, 14, -1},			A,	F,	A,	0}, // 112
	{{10, 76, -1, -1},			C,	C,	B,	0}, // 113
	{{11, 15, -1, -1},			C,	E,	B,	0}, // 114
	{{11, -1, -1, -1},			C,	D,	B,	0}, // 115
	{{13, 16, -1, -1},			A,	D,	B,	0}, // 116
	{{15, 19, 17, -1},			A,	D,	A,	1}, // 117
	{{16, 20, -1, -1},			C,	E,	B,	0}, // 118
	{{19, -1, -1, -1},			C,	E,	B,	0}, // 119
	{{16, 18, -1, -1},			C,	E,	B,	0}, // 120
	{{17, 46, -1, -1},			A,	E,	B,	0}, // 121
	{{26, -1, -1, -1},			C,	B,	S,	0}, // 201
	{{23, 27, -1, -1},			C,	C,	B,	0}, // 202
	{{24, 22, -1, -1},			C,	A,	B,	0}, // 203
	{{32, 25, 31, 23},			A,	C,	B,	0}, // 204
	{{24, 29, -1, -1},			A,	F,	B,	0}, // 205
	{{21, 33, 27, -1},			B,	A,	S,	0}, // 206
	{{22, 26, -1, -1},			C,	C,	B,	0}, // 207
	{{22, -1, -1, -1},			B,	D,	B,	0}, // 208
	{{25, -1, -1, -1},			C,	F,	B,	0}, // 209
	{{31, -1, -1, -1},			B,	D,	B,	0}, // 210
	{{24, 30, -1, -1},			A,	D,	B,	0}, // 211
	{{36, 24, -1, -1},			B,	E,	B,	0}, // 212
	{{26, -1, -1, -1},			C,	A,	B,	1}, // 213
	{{35, -1, -1, -1},			C,	D,	B,	2}, // 214
	{{38, 34, -1, -1},			C,	D,	B,	0}, // 215
	{{45, 32, 38, -1},			C,	A,	A,	1}, // 216
	{{42, -1, -1, -1},			C,	C,	B,	0}, // 217
	{{36, 35, 44, -1},			C,	E,	B,	0}, // 218
	{{40, -1, -1, -1},			C,	E,	B,	0}, // 219
	{{41, 39, 45, -1},			C,	C,	A,	1}, // 220
	{{42, 40, -1, -1},			C,	E,	B,	2}, // 221
	{{37, 41, -1, -1},			C,	D,	B,	1}, // 222
	{{44, -1, -1, -1},			C,	F,	B,	1}, // 223
	{{38, -1, -1, -1},			C,	A,	B,	0}, // 224
	{{40, 36, -1, -1},			C,	A,	B,	1}, // 225
	{{20, 47, 48, -1},			B,	D,	B,	0}, // 301
	{{46, -1, -1, -1},			C,	E,	B,	0}, // 302
	{{46, 51, -1, -1},			C,	E,	B,	0}, // 303
	{{51, 50, 54, -1},			B,	A,	S,	0}, // 304
	{{49, 53, -1, -1},			C,	E,	B,	0}, // 305
	{{48, 49, 52, -1},			C,	D,	B,	1}, // 306
	{{51, 58, -1, -1},			C,	B,	B,	0}, // 307
	{{50, -1, -1, -1},			B,	C,	A,	0}, // 308
	{{49, 56, -1, -1},			C,	C,	B,	0}, // 309
	{{56, -1, -1, -1},			C,	A,	B,	0}, // 310
	{{54, 55, -1, -1},			C,	B,	B,	0}, // 311
	{{58, 60, -1, -1},			C,	D,	B,	0}, // 312
	{{52, 57, 59, -1},			C,	E,	A,	2}, // 313
	{{58, -1, -1, -1},			C,	B,	S,	0}, // 314
	{{57, 66, 61, 63},			A,	A,	B,	2}, // 315
	{{60, -1, -1, -1},			C,	D,	B,	0}, // 316
	{{63, 64, -1, -1},			C,	A,	B,	1}, // 317
	{{60, 62, -1, -1},			C,	B,	B,	2}, // 318
	{{62, -1, -1, -1},			C,	D,	B,	1}, // 319
	{{66, -1, -1, -1},			C,	B,	B,	0}, // 320
	{{60, 65, 67, -1},			A,	D,	A,	0}, // 321
	{{66, -1, -1, -1},			A,	A,	B,	0}, // 322
	{{69, 71, -1, -1},			C,	B,	B,	0}, // 401
	{{68, 75, -1, -1},			A,	B,	B,	0}, // 402
	{{72, -1, -1, -1},			A,	C,	S,	0}, // 403
	{{68, 74, -1, -1},			B,	A,	S,	1}, // 404
	{{76, 70, 75, -1},			A,	E,	A,	0}, // 405
	{{75, -1, -1, -1},			C,	B,	B,	0}, // 406
	{{71, 79, -1, -1},			A,	B,	B,	0}, // 407
	{{72, 69, 73, 80},			B,	D,	B,	1}, // 408
	{{12, 72, 78, -1},			A,	A,	S,	0}, // 409
	{{79, -1, -1, -1},			C,	A,	B,	1}, // 410
	{{76, 81, -1, -1},			A,	A,	S,	0}, // 411
	{{74, 77, 82, -1},			A,	B,	A,	0}, // 412
	{{75, 83, -1, -1},			C,	A,	B,	1}, // 413
	{{78, -1, -1, -1},			C,	B,	B,	2}, // 414
	{{79, 89, -1, -1},			C,	A,	B,	0}, // 415
	{{80, 85, 86, -1},			C,	B,	B,	0}, // 416
	{{86, -1, -1, -1},			B,	D,	B,	0}, // 417
	{{83, -1, -1, -1},			C,	C,	B,	0}, // 418
	{{83, 84, 87, -1},			C,	A,	S,	1}, // 419
	{{86, -1, -1, -1},			B,	C,	B,	0}, // 420
	{{89, -1, -1, -1},			B,	F,	B,	0}, // 501
	{{82, 88, 90, -1},			A,	C,	B,	1}, // 502
	{{89, 91, -1, -1},			C,	D,	B,	1}, // 503
	{{90, 95, -1, -1},			C,	C,	B,	0}, // 504
	{{96, 93, -1, -1},			C,	B,	B,	2}, // 505
	{{92, -1, -1, -1},			C,	B,	B,	1}, // 506
	{{95, -1, -1, -1},			C,	A,	B,	1}, // 507
	{{91, 94, 96, 98},			A,	B,	S,	1}, // 508
	{{95, 92, 97, 100},			A,	A,	A,	0}, // 509
	{{96, -1, -1, -1},			C,	B,	B,	0}, // 510
	{{95, 99, 101, -1},			A,	C,	A,	0}, // 511
	{{98, -1, -1, -1},			C,	A,	S,	0}, // 512
	{{96, 103, -1, -1},		C,	A,	B,	2}, // 513
	{{98, 102, -1, -1},		C,	A,	B,	1}, // 514
	{{101, -1, -1, -1},		C,	B,	S,	0}, // 515
	{{100, -1, -1, -1},		B,	E,	B,	0}, // 516
};

struct probe installed_probes[fn_total];

void install_probes()
{
	int i, index;
	for(i = 0, index = 0; i < sizeof(probes)/sizeof(struct probe); i++)
	{
		while(index < fn_total && probes[i].number > 0)
		{
			struct probe new = {probes[i].type, probes[i].level, 1};
			installed_probes[index++] = new;
			probes[i].number--;
		}
	}
}

void shuffle_probes()
{
	int i, j;
	for(i = 0; i < fn_total - 2; i++)
	{
		j = arc4random_uniform(fn_total - i) + 1;;
		struct probe buf = installed_probes[j];
		installed_probes[j] = installed_probes[i];
		installed_probes[i] = buf;
	}
}

double get_time()
{
    struct timeval t;
    struct timezone tzp;
    gettimeofday(&t, &tzp);
    return t.tv_sec + t.tv_usec*1e-6;
}

int total_production;
int total_revenue;

int get_chain_length(int probe_index)
{
	struct probe current_probe = installed_probes[probe_index];
	int probe_type = current_probe.type;
	if(probe_type != MINING && probe_type != RESEARCH) return 0;

	int probe_level = current_probe.level;

	/* Now branch down and end when not the same. */
	int list[fn_total] = { 0 };
	int list_max = 0, list_index = 0, chain = 0;
	int visited[fn_total] = { 0 };

	list[list_max++] = probe_index;

	for(list_index = 0; list_index <= list_max; list_index++)
	{
		/* If this neighbour is of the same type and level. */
		if(list_index == 0 || (installed_probes[list[list_index]].level == probe_level && installed_probes[list[list_index]].type == probe_type))
		{
			chain++;
			/* add it's neighbours to the queue to check. */
			for(int i = 0; i < 4; i++)
			{
				int next = fn_sites[list[list_index]].connected[i];
				if(next != -1 && visited[next] == 0)
				{
					list[list_max++] = fn_sites[list[list_index]].connected[i];
				}
			}
			visited[list[list_index]] = 1;
		}
	}
	return chain;
}

int best_chain = 0;

void get_totals()
{
	total_production = 0;
	total_revenue = 0;

	for(int i = 0; i < fn_total; i++)
	{
		/* Add the default 50% values. */
		total_production += production_values[fn_sites[i].production] >> 1;
		total_revenue	 += revenue_values[fn_sites[i].revenue] >> 1;

		int chain_length = get_chain_length(i);

		if(chain_length > best_chain)
		{
			best_chain = chain_length;
			printf("LONGEST CHAIN YET = %d\n\n", best_chain);
		}

		float chain_bonus;
		if(chain_length < 3) chain_bonus = 0;
		else if(chain_length < 5) chain_bonus = 0.3;
		else if(chain_length < 8) chain_bonus = 0.5;
		else chain_bonus = 0.8;

		switch(installed_probes[i].type)
		{
			case MINING:
				total_production += (int) floor(production_values[fn_sites[i].production]*(mining_probe_percentages[installed_probes[i].level] + chain_bonus));
				total_revenue	 -= (int) floor(revenue_values[fn_sites[i].revenue]*0.02);
				break;
			case RESEARCH:
				total_revenue	 += (int) floor(revenue_values[fn_sites[i].revenue]*research_probe_percentages[installed_probes[i].level] + chain_bonus);
				total_production -= (int) floor(production_values[fn_sites[i].production]*0.02);
				if(fn_sites[i].sightseeing != 0)
				{
					total_revenue += sightseeing_values[fn_sites[i].revenue]*fn_sites[i].sightseeing;
				}
				break;
		}
	}
}

int main()
{
	srand(time(NULL));
	install_probes();

	int best_production = 0;
	int best_revenue = 0;

	while(1)
	{
		double x = get_time();
		for(int i = 0; i < 100000; i++)
		{
			shuffle_probes();
			get_totals();
			if(total_production + total_revenue > best_production + best_revenue)
			{
				best_production = total_production;
				best_revenue = total_revenue;
				printf("Production: %d, Revenue: %d\n", total_production, total_revenue);
			}
		}
		//printf("%.0f/s\n", 100000.0/(get_time() - x));
	}
	return 0;
}
