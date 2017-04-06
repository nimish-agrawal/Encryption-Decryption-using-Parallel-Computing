// Decryption using Vigenere Cipher
// Input: Message in UPPER CASE, Key in lowercase

#include "mpi.h"
#include <iostream>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <ctype.h>
#define id rank
#define TAG 0

using namespace std;


char vigenere_decrypt(char msg, char k)
{
	if (!isalpha(msg))
		return msg;
	if (msg - k + 96 < 65)
		return char((msg - k + 96) + 26);
	else
		return char(msg - k + 96);
}

int main(int a, char *b[])
{
	MPI_Init(&a, &b);
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status status;

	string str;
	char key[50];
	string decrypted;
	int len, keylen;
	char key_character, decrypted_character, character_to_decrypt;

	if (rank == 0)
	{
		cout << "\n----------------------------------------------------\n";
		cout << "DECRYPTION USING VIGENERE CIPHER";
		cout << "\n----------------------------------------------------\n"; 
		
		cout << "\nEnter the message to be decrypted: ";
		getline(cin, str);
		fflush(stdin);
		len = str.length();
		cout << "\nEnter the cipher key: ";
		cin >> key;
		keylen = strlen(key);
		key[keylen] = '\0';
	}

	MPI_Bcast(&keylen, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (rank == 0)
	{
		for (int i = 1; i <= keylen; i++)
		{
			MPI_Ssend(&key[i - 1], 1, MPI_CHAR, i, TAG, MPI_COMM_WORLD);
		}

		for (int i = 0; i < len; i++)
		{
			MPI_Ssend(&str[i], 1, MPI_CHAR, (i % keylen) + 1, TAG, MPI_COMM_WORLD);
			MPI_Recv(&decrypted_character, 1, MPI_CHAR, (i % keylen) + 1, TAG, MPI_COMM_WORLD, &status);
			decrypted.push_back(decrypted_character);
		}
	}
	else
	{
		MPI_Recv(&key_character, 1, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &status);
		
		if (id <= len%keylen)
		{
			for (int i = 0; i < len / keylen + 1; i++)
			{
				MPI_Recv(&character_to_decrypt, 1, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &status);
				decrypted_character = vigenere_decrypt(character_to_decrypt, key_character);
				MPI_Ssend(&decrypted_character, 1, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
			}
		}
		else
		{
			for (int i = 0; i < len / keylen; i++)
			{
				MPI_Recv(&character_to_decrypt, 1, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &status);
				decrypted_character = vigenere_decrypt(character_to_decrypt, key_character);
				MPI_Ssend(&decrypted_character, 1, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
			}
		}
	}

	if (rank == 0)
	{
		cout << "\nThe decrypted string is: " << decrypted;
		cout << endl;
	}

	MPI_Finalize();

	return 0;
}
