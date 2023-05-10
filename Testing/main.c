#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

int main(){
		printf("Hello Cipher\n");

		char* text = "Secret Key that is 64 char length";
		unsigned char hash[SHA256_DIGEST_LENGTH];
		char* key;
		int i;

		SHA512(text, strlen(text), hash);
		
		key = malloc(SHA256_DIGEST_LENGTH * 2 +1);

		for(i = 0; i < SHA256_DIGEST_LENGTH; i++){
				sprintf(&key[i * 2], "%02x", hash[i]);
		}

		printf("Original : %s\n", text);
		printf("Encrypted : %d\n%s\n", strlen(key), key);

		free(key);

		return 0;
}
