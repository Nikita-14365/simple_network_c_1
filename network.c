#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "network.h"


/*
typedef struct {
	const unsigned int layers, inputs, *neyrones;
	int ***data;
	long (*f)();
	long (*g)();
}  Network;
*/
static void delete_trash(unsigned int, unsigned int *, float ***);

static bool check_args(unsigned int, unsigned int, unsigned int *, float (*)(float), float (*)(float));

Network create_network(unsigned int layers, unsigned int inputs, unsigned int *counts, float (*f)(float), float (*g)(float)) {
	Network trash = {layers, inputs, counts, NULL, f, g};
	if (!check_args(layers, inputs, counts, f, g)) {
		return trash;
	}

	unsigned int *neyrones = malloc(layers * sizeof(unsigned int));
	for (unsigned int i = 0; i < layers; i++) {
		neyrones[i] = counts[i];
	}

	float ***data = malloc(layers * sizeof(float **));

	if (data == NULL) {
		free(neyrones);
		return trash;
	}

	unsigned int inp = inputs;
	for (unsigned int i = 0; i < layers; i++) {
		data[i] = malloc(neyrones[i] * sizeof(float *));
		if (data[i] == NULL) {
			delete_trash(i, neyrones, data);
			free(neyrones);
			return trash;
		}
		for (unsigned int j = 0; j < neyrones[i]; j++) {
			data[i][j] = malloc((inp+1) * sizeof(float));
			//for (unsigned int k = 0; k < inp+2; k++) {
			//	//printf("%i=%i\n", k, data[i][j][k]);
			//}
			if (data[i][j] == NULL) {
				delete_trash(i+1, neyrones, data);
				free(neyrones);
				return trash;
			}
		}
		////printf("%i\n", inp);
		inp = neyrones[i];
	}

	Network network = {layers, inputs, neyrones, data, f, g};
	return network;
}

Network create_network_default(unsigned int layers, unsigned int inputs, unsigned int *counts, float (*f)(float), float (*g)(float), float defaults) {
	Network network = create_network(layers, inputs, counts, f, g);
	
	if (!is_ok_network(network)) {
		return network;
	}

	unsigned int inp = network.inputs;
	for (unsigned int i = 0; i < network.layers; i++) {
		for (unsigned int j = 0; j < network.neyrones[i]; j++) {
			for (unsigned int k = 0; k <= inp; k++) {
				network.data[i][j][k] = defaults;
				if (k == inp) break;
			}
		}
		inp = network.neyrones[i];
	}
	return network;
}

Network create_network_random(unsigned int layers, unsigned int inputs, unsigned int *counts, float (*f)(float), float (*g)(float), float min, float max) {
	Network network = create_network(layers, inputs, counts, f, g);
	
	if (!is_ok_network(network)) {
		return network;
	}

	unsigned int inp = network.inputs;
	for (unsigned int i = 0; i < network.layers; i++) {
		for (unsigned int j = 0; j < network.neyrones[i]; j++) {
			for (unsigned int k = 0; k <= inp; k++) {
				network.data[i][j][k] = (((float) rand()) / RAND_MAX) * (max - min) + min;
				if (k == inp) break;
			}
		}
		inp = network.neyrones[i];
	}
	return network;
}

Network create_network_default_random(unsigned int layers, unsigned int inputs, unsigned int *counts, float (*f)(float), float (*g)(float)) {
	Network network = create_network(layers, inputs, counts, f, g);
	
	if (!is_ok_network(network)) {
		return network;
	}

	unsigned int inp = network.inputs;
	for (unsigned int i = 0; i < network.layers; i++) {
		for (unsigned int j = 0; j < network.neyrones[i]; j++) {
			for (unsigned int k = 0; k <= inp; k++) {
				network.data[i][j][k] = (((float) rand()) / RAND_MAX) * 2 - RAND_MAX;
				if (k == inp) break;
			}
		}
		inp = network.neyrones[i];
	}
	return network;
}

Network copy_network(Network network) {
	if (!is_ok_network(network)) {
		return network;
	}

	Network new_network = create_network(network.layers, network.inputs, network.neyrones, network.f, network.g);
	
	if (!is_ok_network(new_network)) {
		return new_network;
	}

	for (unsigned int i = 0; i < new_network.layers; i++) {
		new_network.neyrones[i] = network.neyrones[i];
	}

	unsigned int inp = new_network.inputs;
	for (unsigned int i = 0; i < new_network.layers; i++) {
		for (unsigned int j = 0; j < new_network.neyrones[i]; j++) {
			for (unsigned int k = 0; k <= inp; k++) {
				new_network.data[i][j][k] = network.data[i][j][k];
				if (k == inp) break;
			}
		}
		inp = new_network.neyrones[i];
	}

	return new_network;
}

void delete_network(Network network) {
	for (unsigned int i = 0; i < network.layers; i++) {
		for (unsigned int j = 0; j < network.neyrones[i]; j++) {
			free(network.data[i][j]);
		}
		free(network.data[i]);
	}
	free(network.data);
	free(network.neyrones);
}

bool is_ok_network(Network network) {
	if (network.layers <= 0 || network.inputs <= 0 || network.neyrones == NULL ||
	    network.data == NULL || network.f == NULL || network.g == NULL) {
		return false;
	}

	for (unsigned int i = 0; i < network.layers; i++) {
		if (network.neyrones[i] <= 0) {
			return false;
		}
	}

	for (unsigned int i = 0; i < network.layers; i++) {
		if (network.data[i] == NULL) {
			return false;
		}
		for (unsigned int j = 0; j < network.neyrones[i]; j++) {
			if (network.data[i][j] == NULL) {
				return false;
			}
		}
	}

	return true;
}

static bool check_args(unsigned int layers, unsigned int inputs, unsigned int *counts, float (*f)(float), float (*g)(float)) {
	if (layers <= 0 || inputs <= 0 || counts == NULL || f == NULL || g == NULL) {
		return false;
	}
	for (unsigned int i = 0; i < layers; i++) {
		if (counts[i] <= 0) {
			return false;
		}
	}
	return true;
}

static void delete_trash(unsigned int layers, unsigned int *neyrones, float ***data) {
	for (unsigned int i = 0; i < layers; i++) {
		if (data[i] != NULL) {
			for (unsigned int j = 0; j < neyrones[i] && data[i][j] != NULL; j++) {
				free(data[i][j]);
			}
			free(data[i]);
		}
	}
	free(data);
}

void save_network(Network network, const char *filename) {
	if (!is_ok_network(network)) {
		return;
	}

	FILE *file = fopen(filename, "w");
	unsigned int inp = network.inputs;
	
	fwrite(&network.layers, sizeof(unsigned int), 1, file);
	fwrite(&network.inputs, sizeof(unsigned int), 1, file);
	fwrite(network.neyrones, sizeof(unsigned int), network.layers, file);
	
	for (unsigned int i = 0; i < network.layers; i++) {
		for (unsigned int j = 0; j < network.neyrones[i]; j++) {
			fwrite(network.data[i][j], sizeof(float), inp+1, file);
		}
		inp = network.neyrones[i];
	}

	fclose(file);
}

Network load_network(const char *filename) {
	FILE *file = fopen(filename, "r");
	unsigned int layers, inputs, *neyrones;

	fread(&layers, sizeof(unsigned int), 1, file);
	fread(&inputs, sizeof(unsigned int), 1, file);
	neyrones = malloc(layers * sizeof(unsigned int));
	fread(neyrones, sizeof(unsigned int), layers, file);

	float ***data = malloc(layers * sizeof(float **));

	unsigned int inp = inputs;
	for (unsigned int i = 0; i < layers; i++) {
		data[i] = malloc((neyrones[i]+1) * sizeof(float *));
		for (unsigned int j = 0; j < neyrones[i]; j++) {
			data[i][j] = malloc((inp+1) * sizeof(float));
			fread(data[i][j], sizeof(float), inp+1, file);
		}
		inp = neyrones[i];
	}

	fclose(file);
	Network network = {layers, inputs, neyrones, data, NULL, NULL};
	return network;
}

float *compute_network(Network network, float input[]) {
	if (!is_ok_network(network)) {
		return NULL;
	}

	float *in = malloc(network.inputs * sizeof(float)), *out;
	for (unsigned int i = 0; i < network.inputs; i++) {
		in[i] = input[i];
	}
	float s;
	unsigned int inp = network.inputs;

	for (unsigned int i = 0; i < network.layers; i++) {
		out = malloc(network.neyrones[i] * sizeof(float));
		for (unsigned int j = 0; j < network.neyrones[i]; j++) {
			//printf("compute_network: data = %d\n", network.data[i][j][0]);
			//printf("compute_network: pow = %d\n", pow(2, sizeof(int)*8-1));
			//printf("compute_network: sum = %d\n", network.data[i][j][0] * pow(2, sizeof(int)*8-1));
			s = network.data[i][j][0];
			//printf("compute1 %f\ncompute2 ", s);
			//printf("compute_network: start s = %i\n", s);
			for (unsigned int k = 0; k < inp; k++) {
				s += in[k] * network.data[i][j][k+1];
			//	printf("%f:%f:%f, ", in[k], network.data[i][j][k+1], in[k] * network.data[i][j][k+1]);
			//	printf("compute_network: middle s = %i\n", s);
			}
			//printf("compute_network: s = %i\n", s);
			out[j] = network.f(s); // s is multiplied by 2**30; out[j] is multiplied by 2**7
			//printf("\ncompute3 %f\n", out[j]);
		}
		free(in);
		in = out;
		inp = network.neyrones[i];
	}

	//printf("compute ");
	//for (unsigned int j = 0; j < network.neyrones[network.layers-1]; j++) {
	//	printf("%f, ", out[j]);
	//}
	//printf("\n");
	return out;
}

void compute_network_to(Network network, float input[], float output[]) {
	float *out = compute_network(network, input);

	if (out == NULL) {
		output = out;
		return;
	}

	for (unsigned int i = 0; i < network.neyrones[network.layers-1]; i++) {
		output[i] = out[i];
	}

	free(out);
}

float teach_network(Network network, float n, float input[], float output[]) {
	if (!is_ok_network(network)) {
		//return (unsigned int) pow(2, sizeof(int)*8);
		return 0;
	}
	
	float error = 0;
	float **errors = malloc(network.layers * sizeof(float *));
	float **values = malloc((network.layers+1) * sizeof(float *));
	for (unsigned int i = 0; i < network.layers; i++) {
		errors[i] = malloc(network.neyrones[i] * sizeof(float));
	}

	float *in = malloc(network.inputs * sizeof(float)), *out;
	for (unsigned int i = 0; i < network.inputs; i++) {
		in[i] = input[i];
	}
	//long *in = malloc(etwork.inputs * sizeof(long)), *out;
	//for (unsigned int i = 0; i < network.inputs; i++) {
	//	in[i] = input[i];
	//}
	float s;
	//printf("teach_network: %i\n", p);
	unsigned int inp = network.inputs;

	for (unsigned int i = 0; i < network.layers; i++) {
		values[i] = in;
		out = malloc(network.neyrones[i] * sizeof(float));
		for (unsigned int j = 0; j < network.neyrones[i]; j++) {
			s = network.data[i][j][0];
			//printf("teach1 %f\nteach2 ", s);
			for (unsigned int k = 0; k < inp; k++) {
				s += in[k] * network.data[i][j][k+1];
			//	printf("%f:%f:%f, ", in[k], network.data[i][j][k+1], in[k] * network.data[i][j][k+1]);
			}
			out[j] = network.f(s);
			//printf("\nteach3 %f\n", out[j]);
		}
		in = out;
		//values[i] = out;
		inp = network.neyrones[i];
	}
	values[network.layers] = out;
	//printf("teach ");
	//for (unsigned int j = 0; j < network.neyrones[network.layers-1]; j++) {
	//	printf("%f, ", out[j]);
	//}
	//printf("\n");

//	for (unsigned int i = 0; i < network.layers; i++) {
//		for (unsigned int j = 0; j < network.neyrones[network.layers-i-1]; j++) {
//			errors[network.layers-i-1][j] = 
	
	for (unsigned int j = 0; j < network.neyrones[network.layers-1]; j++) {
		s = output[j] - out[j];
		if (s < 0) {
			s = -s;
		}
		////errors[network.layers-1][j] = output[j] - out[j];
		//////errors[network.layers-1][j] = out[j] - output[j];
		//errors[network.layers-1][j] = (output[j] - out[j]) * out[j] * (1.0 - out[j]);
		errors[network.layers-1][j] = (output[j] - out[j]) * network.g(out[j]);
		error += s;
		printf("error: %f - %f = %f\n", output[j], out[j], s);
	}

	for (unsigned int i = network.layers-2; i >= 0; i--) {
		for (unsigned int j = 0; j < network.neyrones[i]; j++) {
			s = 0.0;
			for (unsigned int k = 0; k < network.neyrones[i+1]; k++) {
				//printf("teach_network: p %i\n", p);
				s += network.data[i+1][k][j+1] * errors[i+1][k];
			}
			errors[i][j] = s * network.g(values[i+1][j]);
		}
		if (i == 0) break;
	}
	
	inp = network.inputs;
	for (unsigned int i = 0; i < network.layers; i++) {
		for (unsigned int j = 0; j < network.neyrones[i]; j++) {
			//s = n * errors[i][j] * network.g(values[i+1][j]);
			s = n * errors[i][j];
			//printf("A%i\n", s);
			network.data[i][j][0] += s;
			for (unsigned int k = 0; k < inp; k ++) {
				//printf("teach_network: p2 %i\n", p*p*p);
				network.data[i][j][k+1] += s * values[i][k];
				//printf("B%i\n", values[i][k]);
				//printf("CONTINUE\n");
			}
		}
		inp = network.neyrones[i];
	}

	for (unsigned int i = 0; i < network.layers; i++) {
		free(errors[i]);
		//printf("free\n");
		//printf("%p, %p, %p, %p\n", values[i], NULL, malloc(0), errors[i]);
		free(values[i]);
		//printf("free2\n");
	}
	free(errors);
	//printf("free3\n");
	free(values);
	//printf("free4\n");
	free(out);
	//printf("free5\n");

	//printf("teach_network, all is good\n");
	return error;
}
