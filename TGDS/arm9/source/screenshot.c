#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include "screenshot.h"
#include "GBA.h"
#include "posixHandleTGDS.h"

int ofs=0;
u8 *pbuf;
volatile u8 buf[ScreenWidth*ScreenHeight*3];

//screenshot functions from moonshell2
u8* CreateBMPImage()
{
  ofs = 0;
  
  u32 linelen;
  int size = ScreenHeight*ScreenWidth*3;
  linelen=ScreenWidth*3;
  linelen=(linelen+3)&(~3);
  
  u32 bufsize=14+40+(ScreenHeight*linelen);
  
  
  //u8 *pbuf=(u8*)malloc(bufsize);
  pbuf=(u8*)&buf[0];
  
  if(pbuf==NULL){
    printf("memory overflow!! CreateBMPImage. malloc(%d)=NULL; ",bufsize);
	while(1);
    return (u8*)0;
  }
  
  u32 ofs=0;
  
  // BITMAPFILEHEADER
  
  // bfType 2 byte �t�@�C���^�C�v 'BM' - OS/2, Windows Bitmap
  add8((u8)'B');
  add8((u8)'M');
  // bfSize 4 byte �t�@�C���T�C�Y (byte)
  add32(bufsize);
  // bfReserved1 2 byte �\��̈� ��� 0
  add16(0);
  // bfReserved2 2 byte �\��̈� ��� 0
  add16(0);
  // bfOffBits 4 byte �t�@�C���擪����摜�f�[�^�܂ł̃I�t�Z�b�g (byte)
  add32(14+40);
  
  // BITMAPINFOHEADER
  
  // biSize 4 byte ���w�b�_�̃T�C�Y (byte) 40
  add32(40);
  // biWidth 4 byte �摜�̕� (�s�N�Z��)
  add32(ScreenWidth);
  // biHeight 4 byte �摜�̍��� (�s�N�Z��) biHeight �̒l�������Ȃ�C�摜�f�[�^�͉�������
  add32(ScreenHeight);
  // biPlanes 2 byte �v���[���� ��� 1
  add16(1);
  // biBitCount 2 byte 1 ��f������̃f�[�^�T�C�Y (bit)
  add16(24);
  // biCopmression 4 byte ���k�`�� 0 - BI_RGB (�����k)
  add32(0);
  // biSizeImage 4 byte �摜�f�[�^���̃T�C�Y (byte) 96dpi �Ȃ��3780
  add32(0);
  // biXPixPerMeter 4 byte �������𑜓x (1m������̉�f��) 96dpi �Ȃ��3780
  add32(0);
  // biYPixPerMeter 4 byte �c�����𑜓x (1m������̉�f��) 96dpi �Ȃ��3780
  add32(0);
  // biClrUsed 4 byte �i�[����Ă���p���b�g�� (�g�p�F��) 0 �̏ꍇ������
  add32(0);
  // biCirImportant 4 byte �d�v�ȃp���b�g�̃C���f�b�N�X 0 �̏ꍇ������
  add32(0);
  
  int y=0;
  u16 * vram_src = (u8*)0x06010000;
  for(y=ScreenHeight-1;0<=y;y--){
    u16 * psrcbm=/*&VRAMBuf*/(u16 *)vram_src[y*ScreenWidth];
    int x = 0;
	for(x=0;x<ScreenWidth;x++){
      u16 col=*psrcbm++;
      u8 b=(col>>10)&0x1f;
      u8 g=(col>>5)&0x1f;
      u8 r=(col>>0)&0x1f;
      add8(b<<3);
      add8(g<<3);
      add8(r<<3);
    }
	
	for(x=0;x<(linelen-(ScreenWidth*3));x++){
      add8(0);
    }
  }
  
  
  writebuf2file("fat:/arm7.bmp",(u8*)&buf[0],sizeof(buf));
  
  return(pbuf);
}


int writebuf2file(char * filename,u8 * buf,int size){
    
	//w+
	FILE * fh_dump = fopen(filename,"w+");
	int sizewritten=fwrite((u8*)buf, 1, size, fh_dump);
	
	if(sizewritten > 0)
		printf("write ok!  ");
	else{
		printf("write was 0.. :(");
	}
	
	fclose(fh_dump);
    
    return sizewritten;
}