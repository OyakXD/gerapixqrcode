#include <qrencode.h>
#include "../libcrc/include/checksum.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

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
		(unsigned long)(14 + 4 + strlen(chave)),
		(unsigned long)strlen(chave),
		chave
	);

	uint16_t crc = crc_ccitt_ffff((unsigned char *)pix, strlen(pix));
	sprintf(pix + strlen(pix), "%04x", crc);
}

int main()
{
	char pix[300];

	gerarPix("sua_chave_pix", pix);
	
	QRcode *qrcode = QRcode_encodeString(pix, 0, QR_ECLEVEL_L, QR_MODE_8, 1);

	int width = qrcode->width;

	for(int y = 0; y < width; y++)
	{
		for(int x = 0; x < width; x++)
		{
			if(qrcode->data[y * width + x] & 1)
			{
				printf("\u2588\u2588");
			} else {
				printf("  ");
			}
		}
		printf("\n");
	}

	QRcode_free(qrcode);
	return 0;
}
