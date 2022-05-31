#include <stdio.h>
#include <stdlib.h>


int codelen(char* code)
{
	int l = 0;
	while (*(code + l) != '\0')
		l++;
	return l;
}


void strconcat(char* str, char* parentcode, char add)
{
	int i = 0;
	while (*(parentcode + i) != '\0')
	{
		*(str + i) = *(parentcode + i);
		i++;
	}
	if (add != '2')
	{
		str[i] = add;
		str[i + 1] = '\0';
	}
	else
		str[i] = '\0';
}


int fib(int n)
{
	if (n <= 1)
		return n;
	return fib(n - 1) + fib(n - 2);
}


int main()
{
	int i, j;
	char filename[] = "samp.bmp";
	int data = 0, offset, bpp = 0, width, height;
	long bmpsize = 0, bmpdataoff = 0;
	int** image;
	int temp = 0;

	
	FILE* image_file;
	
	image_file = fopen(filename, "rb");
	if (image_file == NULL)
	{
		printf("Error Opening File!!");
		exit(1);
	}
	else
	{
		
	
		offset = 0;
		
	
		offset = 2;
		
		fseek(image_file, offset, SEEK_SET);
		
	
		fread(&bmpsize, 4, 1, image_file);
		

		offset = 10;
		
		fseek(image_file, offset, SEEK_SET);
		
	
		fread(&bmpdataoff, 4, 1, image_file);
		
		fseek(image_file, 18, SEEK_SET);
		
		fread(&width, 4, 1, image_file);
		
		fread(&height, 4, 1, image_file);
		
		fseek(image_file, 2, SEEK_CUR);
		
		fread(&bpp, 2, 1, image_file);
		
		fseek(image_file, bmpdataoff, SEEK_SET);
		
		image = (int**)malloc(height * sizeof(int*));
		
		for (i = 0; i < height; i++)
		{
			image[i] = (int*)malloc(width * sizeof(int));
		}
		
		int numbytes = (bmpsize - bmpdataoff) / 3;
		
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				fread(&temp, 3, 1, image_file);
				temp = temp & 0x0000FF;
				image[i][j] = temp;
			}
		}
	}
	
	int hist[256];
	for (i = 0; i < 256; i++)
		hist[i] = 0;
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
			hist[image[i][j]] += 1;
			
	int nodes = 0;
	for (i = 0; i < 256; i++)
		if (hist[i] != 0)
			nodes += 1;
			
	float p = 1.0, ptemp;
	for (i = 0; i < 256; i++)
	{
		ptemp = (hist[i] / (float)(height * width));
		if (ptemp > 0 && ptemp <= p)
			p = ptemp;
	}
	
	i = 0;
	while ((1 / p) > fib(i))
		i++;
	int maxcodelen = i - 3;
	
	struct pixfreq
	{
		int pix, larrloc, rarrloc;
		float freq;
		struct pixfreq *left, *right;
		char code[maxcodelen];
	};
	
	struct huffcode
	{
		int pix, arrloc;
		float freq;
	};
	
	struct pixfreq* pix_freq;
	struct huffcode* huffcodes;
	int totalnodes = 2 * nodes - 1;
	pix_freq = (struct pixfreq*)malloc(sizeof(struct pixfreq) * totalnodes);
	huffcodes = (struct huffcode*)malloc(sizeof(struct huffcode) * nodes);
	
	j = 0;
	int totpix = height * width;
	float tempprob;
	for (i = 0; i < 256; i++)
	{
		if (hist[i] != 0)
		{
			
			huffcodes[j].pix = i;
			pix_freq[j].pix = i;
			
			huffcodes[j].arrloc = j;
			
			tempprob = (float)hist[i] / (float)totpix;
			pix_freq[j].freq = tempprob;
			huffcodes[j].freq = tempprob;
			
			pix_freq[j].left = NULL;
			pix_freq[j].right = NULL;
			
			pix_freq[j].code[0] = '\0';
			j++;
		}
	}
	
	struct huffcode temphuff;
	
	for (i = 0; i < nodes; i++)
	{
		for (j = i + 1; j < nodes; j++)
		{
			if (huffcodes[i].freq < huffcodes[j].freq)
			{
				temphuff = huffcodes[i];
				huffcodes[i] = huffcodes[j];
				huffcodes[j] = temphuff;
			}
		}
	}
	
	float sumprob;
	int sumpix;
	int n = 0, k = 0;
	int nextnode = nodes;
	
	while (n < nodes - 1)
	{
		
		sumprob = huffcodes[nodes - n - 1].freq + huffcodes[nodes - n - 2].freq;
		sumpix = huffcodes[nodes - n - 1].pix + huffcodes[nodes - n - 2].pix;
		
		pix_freq[nextnode].pix = sumpix;
		pix_freq[nextnode].freq = sumprob;
		pix_freq[nextnode].left = &pix_freq[huffcodes[nodes - n - 2].arrloc];
		pix_freq[nextnode].right = &pix_freq[huffcodes[nodes - n - 1].arrloc];
		pix_freq[nextnode].code[0] = '\0';
		i = 0;
		
		while (sumprob <= huffcodes[i].freq)
			i++;
			
		for (k = nodes; k >= 0; k--)
		{
			if (k == i)
			{
				huffcodes[k].pix = sumpix;
				huffcodes[k].freq = sumprob;
				huffcodes[k].arrloc = nextnode;
			}
			else if (k > i)
			
				huffcodes[k] = huffcodes[k - 1];
			
		}
		n += 1;
		nextnode += 1;
	}
	
	char left = '0';
	char right = '1';
	int index;
	for (i = totalnodes - 1; i >= nodes; i--)
	{
		if (pix_freq[i].left != NULL)
			strconcat(pix_freq[i].left->code, pix_freq[i].code, left);
		if (pix_freq[i].right != NULL)
			strconcat(pix_freq[i].right->code, pix_freq[i].code, right);
	}
	
	int pix_val;
	int l;
	
	FILE* imagehuff = fopen("encoded_image.txt", "wb");
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
		{
			pix_val = image[i][j];
			for (l = 0; l < nodes; l++)
				if (pix_val == pix_freq[l].pix)
					fprintf(imagehuff, "%s", pix_freq[l].code);
		}
		
	printf("Huffmann Codes::\n\n");
	printf("pixel values -> Code\n\n");
	for (i = 0; i < nodes; i++) {
		if (snprintf(NULL, 0, "%d", pix_freq[i].pix) == 2)
			printf("	 %d	 -> %s\n", pix_freq[i].pix, pix_freq[i].code);
		else
			printf(" %d	 -> %s\n", pix_freq[i].pix, pix_freq[i].code);
	}
	
	float avgbitnum = 0;
	for (i = 0; i < nodes; i++)
		avgbitnum += pix_freq[i].freq * codelen(pix_freq[i].code);
	printf("Average number of bits:: %f", avgbitnum);
}

