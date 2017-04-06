// CUDA implementation of Vigenere cipher
// Input: Message in UPPER CASE, Key in lowercase

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include<stdio.h>
#include<iostream>
#include<string.h>
#include<string>
#include<math.h>
#include<stdlib.h>

using namespace std;

__global__ void vigenere_encrypt(char *strptr, char *keyptr, char *encryptedptr, int *lenptr, int *keylenptr)
{
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	int j = i % (*keylenptr - 1);
	
	if (strptr[i] < 65 || (strptr[i] > 90 && strptr[i] < 97) || strptr[i] > 122)
		encryptedptr[i] = strptr[i];
	else
	{
		if (strptr[i] + keyptr[j] - 96 > 90)
			encryptedptr[i] = char(strptr[i] + keyptr[j] - 96 - 26);
		else
			encryptedptr[i] = char(strptr[i] + keyptr[j] - 96);
	}
}

__global__ void vigenere_decrypt(char *strptr, char *keyptr, char *decryptedptr, int *lenptr, int *keylenptr)
{
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	int j = i % (*keylenptr - 1);

	if (strptr[i] < 65 || (strptr[i] > 90 && strptr[i] < 97) || strptr[i] > 122)
		decryptedptr[i] = strptr[i];
	else
	{
		if (strptr[i] - keyptr[j] + 96 < 65)
			decryptedptr[i] = char(strptr[i] - keyptr[j] + 96 + 26);
		else
			decryptedptr[i] = char(strptr[i] - keyptr[j] + 96);
	}
}

int main()
{
	cout << "\n----------------------------------------------------\n";
	cout << "ENCRYPTION AND DECRYPTION USING VIGENERE CIPHER";
	cout << "\n----------------------------------------------------\n\n";
	cout << "What do you want to do?\n1. Encrypt\n2. Decrypt\nAnything else to exit\n\n";
	int choice;
	fflush(stdin);

	while(1)
	{
		cout << "Enter choice: ";
		cin >> choice;

		if (choice != 1 && choice != 2)
		{
			cout << "\nQuitting!\n\n";
			exit(0);
		}

		if (choice == 1)
		{
			char str[500], key[26];
			char encrypted[500];
			int len, keylen;
			getchar();
			fflush(stdin);
		
			cout << "Enter the messasge to be encrypted: ";
			fgets(str, 500, stdin);
			fflush(stdin);
			len = strlen(str);

			cout << "Enter the cipher key: ";
			fgets(key, 26, stdin);
			keylen = strlen(key);

			char *strptr, *keyptr, *encryptedptr;
			int *lenptr, *keylenptr;

			int strsize = len * sizeof(char);
			int keysize = keylen * sizeof(char);

			cudaMalloc((void**)&strptr, strsize);
			cudaMalloc((void**)&keyptr, keysize);
			cudaMalloc((void**)&encryptedptr, strsize);
			cudaMalloc((void**)&lenptr, sizeof(int));
			cudaMalloc((void**)&keylenptr, sizeof(int));

			cudaMemcpy(strptr, &str, strsize, cudaMemcpyHostToDevice);
			cudaMemcpy(keyptr, &key, keysize, cudaMemcpyHostToDevice);
			cudaMemcpy(lenptr, &len, sizeof(int), cudaMemcpyHostToDevice);
			cudaMemcpy(keylenptr, &keylen, sizeof(int), cudaMemcpyHostToDevice);
			
			int no_of_blocks = ceil(sqrt(len));
			// cout << "Blocks " << no_of_blocks << endl;
			vigenere_encrypt << <no_of_blocks, no_of_blocks >> > (strptr, keyptr, encryptedptr, lenptr, keylenptr);

			cudaMemcpy(encrypted, encryptedptr, strsize, cudaMemcpyDeviceToHost);

			encrypted[len] = '\0';
			cout << "The encrypted string is: ";
			for (int i = 0; i < len; i++)
				cout << encrypted[i];
			cout << endl;

			cudaFree(strptr);
			cudaFree(keyptr);
			cudaFree(encryptedptr);
			cudaFree(lenptr);
			cudaFree(keylenptr);
		}

		else
		{
			char str[500], key[26];
			char decrypted[500];
			int len, keylen;
			getchar();
			fflush(stdin);
			
			cout << "Enter the messasge to be decrypted: ";
			fgets(str, 500, stdin);
			fflush(stdin);
			len = strlen(str);

			cout << "Enter the cipher key: ";
			fgets(key, 26, stdin);
			keylen = strlen(key);

			char *strptr, *keyptr, *decryptedptr;
			int *lenptr, *keylenptr;

			int strsize = len * sizeof(char);
			int keysize = keylen * sizeof(char);

			cudaMalloc((void**)&strptr, strsize);
			cudaMalloc((void**)&keyptr, keysize);
			cudaMalloc((void**)&decryptedptr, strsize);
			cudaMalloc((void**)&lenptr, sizeof(int));
			cudaMalloc((void**)&keylenptr, sizeof(int));

			cudaMemcpy(strptr, &str, strsize, cudaMemcpyHostToDevice);
			cudaMemcpy(keyptr, &key, keysize, cudaMemcpyHostToDevice);
			cudaMemcpy(lenptr, &len, sizeof(int), cudaMemcpyHostToDevice);
			cudaMemcpy(keylenptr, &keylen, sizeof(int), cudaMemcpyHostToDevice);

			int no_of_blocks = ceil(sqrt(len));
			// cout << "Blocks " << no_of_blocks << endl;

			vigenere_decrypt << <no_of_blocks, no_of_blocks >> > (strptr, keyptr, decryptedptr, lenptr, keylenptr);

			cudaMemcpy(decrypted, decryptedptr, strsize, cudaMemcpyDeviceToHost);

			decrypted[len] = '\0';
			cout << "The decrypted string is: ";
			for (int i = 0; i < len; i++)
				cout << decrypted[i];
			cout << endl;

			cudaFree(strptr);
			cudaFree(keyptr);
			cudaFree(decryptedptr);
			cudaFree(lenptr);
			cudaFree(keylenptr);
		}
	}
	
	return 0;
}
