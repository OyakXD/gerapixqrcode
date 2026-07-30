#include <qrencode.h>
#include "../libcrc/include/checksum.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <png.h>

void escreverPNG(QRcode *qrcode, FILE *fp)
{
	setvbuf(stdout, NULL, _IONBF, 0);

	const int scale = 10;
	const int border = 4;

	int qrsize = qrcode->width;
	int size = (qrsize + border * 2) * scale;

	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info = png_create_info_struct(png);

	png_init_io(png, fp);

	png_set_IHDR(
		png,
		info,
		size,
		size,
		8,
		PNG_COLOR_TYPE_GRAY,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT		
	);


	png_write_info(png, info);

	png_bytep row = malloc(size);

	for (int y = 0; y < size ; y++)
	{
		for (int x = 0; x < size ; x++)
		{
			int mx = x / scale - border;
			int my = y / scale - border;

			unsigned char pixel = 255; // branco
			
			if (mx >= 0 && mx < qrsize && my >= 0 && my < qrsize)
			{
				if (qrcode->data[my * qrsize + mx] & 1) 
				{
					pixel = 0;
				}	
			}

			row[x] = pixel;
		}

		png_write_row(png, row);
	}

	png_write_end(png, NULL);

	free(row);
	png_destroy_write_struct(&png, &info);
}

// Digita sua chave pix e obtenha a saída em *pix
void gerarPix(const char *chave, char *pix)
{
	/* Esse tipo de string é o PAYLOAD EMV do PIX (BR Code), usado em QRcode estático
	 * A estrutura é padronizada pelo Banco Central (EMVCo).
	 *
	 * Os campos são compostos são os TLV (Tag-Length-Value).
	 *
	 * Tag | Significado
	 * 00    Payload Format Indicator
	 * 26    Merchant Account Information (PIX)
	 * 52    Merchant Category Code
	 * 53    Moeda (986=BRL)
	 * 58    País (BR)
	 * 59    Nome do recebedor
	 * 60    Cidade
	 * 62    Campo adicional (opcional)
	 * 63    CRC16 (a libcrc foi usada para calcular o valor do CRC16)
	 * */
	
	
	sprintf(
		pix,
		"00020126%02lu0014BR.GOV.BCB.PIX01%02lu%s5204000053039865802BR5901N6001C62070503***6304",
		(unsigned long)(22 + strlen(chave)),
		(unsigned long)strlen(chave),
		chave
	);

	uint16_t crc = crc_ccitt_ffff((unsigned char *)pix, strlen(pix));
	sprintf(pix + strlen(pix), "%04X", crc);
}

int main()
{
	char body[256];
	if(fgets(body, sizeof(body), stdin) == NULL) 	// body = "chave=sua_chave_pix"
		return 1;				

	char *chave = strstr(body, "chave=");
	if (chave) {
		chave += 6; // pula "chave="
		chave[strcspn(chave, "\r\n")] = '\0';
	}


	char pix[300];

	gerarPix(chave, pix);
	
	QRcode *qrcode = QRcode_encodeString(pix, 0, QR_ECLEVEL_L, QR_MODE_8, 1);

	escreverPNG(qrcode, stdout);

	QRcode_free(qrcode);

	return 0;
}
