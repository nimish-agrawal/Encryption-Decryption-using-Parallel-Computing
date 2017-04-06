// Encryption using Vigenere Cipher
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

char vigenere(char msg, char k)
{
	if (!isalpha(msg))
		return msg;
	if (msg + k - 96 > 90)
		return char((msg + k - 96) - 26);
	else
		return char(msg + k - 96);
}

int main(int a, char *b[])
{
	MPI_Init(&a, &b);
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status status;
	string str;
	// char str[200];
	char key[50];
	string encrypted;
	int len, keylen;
	char key_character, encrypted_character, character_to_encrypt;
	
	if (rank == 0)
	{
		cout << "\n----------------------------------------------------\n";
		cout << "ENCRYPTION USING VIGENERE CIPHER";
		cout << "\n----------------------------------------------------\n";
		
		cout << "\nEnter the message to be encrypted: ";
		//fgets(str, 200, stdin);// >> str;
		getline(cin, str);
		fflush(stdin);
		len = str.length();
		//len = strlen(str);
		// str[len] = '\0';
		cout << "\nEnter the cipher key: ";
		cin >> key;
		keylen = strlen(key);
		key[keylen] = '\0';
	}

	MPI_Bcast(&keylen, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (rank == 0)
	{
		// Sending key - one character to one process
		// cout << "Inside root process\n";
		for (int i = 1; i <= keylen; i++)
		{
			// cout << "Sending " << key[i - 1] << " to " << i << endl;
			MPI_Ssend(&key[i - 1], 1, MPI_CHAR, i, TAG, MPI_COMM_WORLD);
		}

		// Sending message, minimum len/keylen characters to a process
		for (int i = 0; i < len; i++)
		{
			// cout << "Sending " << str[i] << " to " << (i % keylen) + 1 << endl;
			MPI_Ssend(&str[i], 1, MPI_CHAR, (i % keylen) + 1, TAG, MPI_COMM_WORLD);
			// cout << "Receiving encrypted character: ";
			MPI_Recv(&encrypted_character, 1, MPI_CHAR, (i % keylen) + 1, TAG, MPI_COMM_WORLD, &status);
			// cout << "Received encrypted: " << encrypted_character << " in root\n";
			encrypted.push_back(encrypted_character);
			// append to encrypted message
		}
		// cout << "Exiting root process\n";
	}
	else
	{
		// cout << "Inside process-" << id << endl;
		// Receive key character
		MPI_Recv(&key_character, 1, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &status);
		// cout << "Process-" << id << " received " << key_character << endl;
		
		if (id <= len%keylen)
		{
			for (int i = 0; i < len / keylen + 1; i++)
			{
				// Receive character, encrypt and send to root
				// cout << "Process-" << id << " is trying to receive " << endl;
				MPI_Recv(&character_to_encrypt, 1, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &status);
				encrypted_character = vigenere(character_to_encrypt, key_character);
				MPI_Ssend(&encrypted_character, 1, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
			}
		}
		else
		{
			for (int i = 0; i < len / keylen; i++)
			{
				// Receive character, encrypt and send to root
				// cout << "Process-" << id << " is trying to receive " << endl;
				MPI_Recv(&character_to_encrypt, 1, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &status);
				encrypted_character = vigenere(character_to_encrypt, key_character);
				MPI_Ssend(&encrypted_character, 1, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
			}
		}
		// cout << "Exiting process-" << id << endl;
	}

	if (rank == 0)
	{
		cout << "\nThe encrypted string is: " << encrypted;
		cout << endl;
	}

	MPI_Finalize();
	return 0;
}
