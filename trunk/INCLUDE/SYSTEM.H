/*		SYSTEM.H
		MUST BE INCLUDED ON ALL C FILES		*/

//INCLUDED FILES
#include		<FORMATTING.H>
#include		<COMMONVARS.H>
#include		<stdarg.h>
		
#ifndef				SYSTEM_H_INCLUDED
	#define				SYSTEM_H_INCLUDED

//MAIN.C
	void				iError(char *Reason);
	
//TIMER.C
	void				display_uptime(void);
	void				timer_wait(int ticks);
	void				sleep(int secs);
	void				timer_install(void);

//STRING.C
	int					ASCII2int(char A);
	char				int2ASCII(int A);
	bool				arrayAppend(char* out, const char in);
	bool				arrayRemove(char* out, const int cs);
	int					searchChar(char find, char *in);
	int 				explode( char out[50][100], const char *s1, const char s2 );
	bool				streql( const char *s1, const char *s2 );
	int					strcmp( const char * s1, const char * s2 );
	char				*strcpy(char *s1, const char *s2);
	char*				StringtoUpper(char* in);
	char*				StringtoLower(char* in);
	char				ChartoUpper(char in);
	char				ChartoLower(char in);
	char*				strchr (char * str, int character );
	char*				charchange(const char in);
	size_t				strlen(const char *str);
	int					stringf(char *str, const char *format, ...);
	void				itoa(unsigned i,unsigned base,char* buf);
	void				itoa_s(int i,unsigned base,char* buf);

//STDIO.C
	void				initVideo(void);
	void				print(char *message);
	void				printf(char *message, ...);
	void				putch(char charactor);
	void				movcur(int x, int y);
	void				scrollScreen();
	void				cls();
	void				setColor(int cSet);
	int					getColor(void);
	unsigned char 		inb (unsigned short _port);
	void 				outb (unsigned short _port, unsigned char _data);
	void				*memcpy(void *dest, const void *src, size_t count);
	void				*memset(void *dest, char val, size_t count);
	unsigned short		*memsetw(unsigned short *dest, unsigned short val, size_t count);

//GDT.C
	void				gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
	void				gdt_install(void);

//INT.C
	void				idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
	void				idt_install(void);
	void				isrs_install(void);
	void				irq_install_handler(int irq, void (*handler)(struct regs *r));
	void				irq_uninstall_handler(int irq);
	void				irq_install(void);

//KEYBOARD.C
	void				install_keyboard(void);

//CMD.C
	void				cmd_handler(void);
	void				init_cmd(void);
	
//MATH.C
	int					exponent(int base, unsigned int exp);
	unsigned int		textTOhex(char* s);
	char*				hexTOtext(int in);
	char*				intTOchar(int in);
	unsigned int		charTOint(char* in);
	uint64_t			BIN2INT(unsigned char* in);
	bool				isEven(uint64_t in);
	
//FLOPPY.C
	void				floppy_set_dma (int addr);
	bool floppy_init_dma(uint8_t* buffer, unsigned length);
	uint8_t*			floppy_readSector (int sectorLBA, int RAMlocation, uint8_t secotrs);
	void				floppy_install(void);
	void				lbaCHS (int lba,int *head,int *track,int *sector);
	void				floppy_reset(void);
	void				floppy_motor(bool on);
	
//DMA.C
	void				dma_set_mode (uint8_t channel, uint8_t mode);
	void				dma_set_read (uint8_t channel);
	void 				dma_set_write (uint8_t channel);
	void 				dma_set_address(uint8_t channel, uint8_t low, uint8_t high);
	void 				dma_set_count(uint8_t channel, uint8_t low, uint8_t high);
	void 				dma_mask_channel (uint8_t channel);
	void 				dma_unmask_channel (uint8_t channel);
	void 				dma_reset_flipflop (int dma);
	void 				dma_reset (int dma);
	void 				dma_set_external_page_register (uint8_t reg, uint8_t val);
	void 				dma_unmask_all (int dma);
	
//CMOS.C
	void				readCMOS(void);
	char				*CMOS_Floppy_Decode(int type);
	
//FAT.C
	void BPB_init(void);
	/**/uint32_t BPB_TotalSectors(void);
	int BPB_ReservedSectors(void);
	int BPB_FATs(void);
	int BPB_SectorsFAT(void);
	uint16_t BPB_Roots(void);
	uint8_t BPB_SectorsCuster(void);
	uint16_t FAT_ClusterData(int cluster);
	uint16_t FAT_Cluster2Sector(int cluster);
	void ROOT_entryFileName(unsigned int entry, char ret[10]);
	void ROOT_entryFileExt(unsigned int entry, char ret[5]);
	uint16_t ROOT_entryStartCluster(unsigned int entry);
	uint32_t ROOT_entrySize(unsigned int entry);/**/
	void FAT_init(void);	
	void ROOT_init(void);
	bool ROOT_neverUsed(unsigned int entry);
	bool ROOT_isFree(unsigned int entry);
	bool ROOT_isEntry(unsigned int entry);
	bool FAT_FILENAME(char in[15], char FN[10], char FE[5]);
	uint16_t FAT_SEARCH_ROOT(char FN[10], char FE[5]);
	void DIR_FAT_ROOT(void);
	void FAT_LOAD(uint16_t startcluster, unsigned char process);
	
//MEM.C
	uint64_t detectRAM(void);
	void initMAP(void);
	void initMEM(void);
	void printMEMMAP(void);
	uint64_t freeRAM(void);
	uint64_t reservedRAM(void);
	uint64_t usedRAM(void);
	uint64_t findFreeRAM(int kbytes);
	uint64_t findRAM(unsigned char process);
	uint64_t sizeofRAM(unsigned char process);
	void allocateRAM(uint64_t start, int kbytes, unsigned char process);
	void unallocateRAM(unsigned char process);
	
	

#endif
