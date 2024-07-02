#include "protocol.h"

//---------------Checksum sul dato a 8 bit ----------------
void Check_sum (uint8_t Dato, uint8_t *Crc)
{
    uint8_t n, x ;
    for ( n = 0 ; n < 8 ; n++ )
    {
        x = ((Dato << n) ^(*Crc)); 
        x &= (0x80) ;		  		
        if ( x )
            *Crc |= (0x80) ;			
        else
            *Crc &= (0x7F);			
        if ( ((*Crc << 1)& 0x80)^x )     
            *Crc |= (0x40);			
        else
            *Crc &= (0xBF  );			
        if ( ((*Crc << 6)& 0x80)^x )     
            *Crc |= (0x02);			
        else
            *Crc &= (0xFD);
        *Crc = *Crc << 1  ;			
        if ( x )
            *Crc |= (0x01);               	
    }						 
}

// Routine di calcolo del CRC
//---------------Checksum sul "N" dati ----------------
uint8_t Check_nby (uint8_t N_by, uint8_t *Punt )
{
	uint8_t n,Valore_crc;
	Valore_crc =0;
	for ( n = 0; n < N_by ; n++ )
		Check_sum(*(Punt+n),&Valore_crc);  
	return Valore_crc; 
}