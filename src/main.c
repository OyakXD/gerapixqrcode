#include <qrencode.h>
#include <stdio.h>

int main()
{
	QRcode *qrcode = QRcode_encodeString("teste", 0, QR_ECLEVEL_L, QR_MODE_8, 1);

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
