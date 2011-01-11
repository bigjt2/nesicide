//    NESICIDE - an IDE for the 8-bit NES.
//    Copyright (C) 2009  Christopher S. Pow

//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "cnesrommapper009.h"

#include "cnesppu.h"

// MMC2 stuff
uint8_t  CROMMapper009::m_reg [] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t  CROMMapper009::m_latch0 = 0xFE;
uint8_t  CROMMapper009::m_latch1 = 0xFE;
uint8_t  CROMMapper009::m_latch0FD = 0;
uint8_t  CROMMapper009::m_latch0FE = 1;
uint8_t  CROMMapper009::m_latch1FD = 0;
uint8_t  CROMMapper009::m_latch1FE = 0;

CROMMapper009::CROMMapper009()
{

}

CROMMapper009::~CROMMapper009()
{

}

void CROMMapper009::RESET ()
{
   m_mapper = 9;

   CROM::RESET ( m_mapper );

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 0 ];
   m_PRGROMbank [ 0 ] = 0;
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ m_numPrgBanks-3 ];
   m_PRGROMbank [ 1 ] = m_numPrgBanks-3;
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ m_numPrgBanks-2 ];
   m_PRGROMbank [ 2 ] = m_numPrgBanks-2;
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ m_numPrgBanks-1 ];
   m_PRGROMbank [ 3 ] = m_numPrgBanks-1;

   m_latch0 = 0xFE;
   m_latch1 = 0xFE;
   m_latch0FD = 0;
   m_latch0FE = 1;
   m_latch1FD = 0;
   m_latch1FE = 0;

   m_pCHRmemory [ 0 ] = m_CHRROMmemory [ (m_latch0FE>>1) ] + ((m_latch0FE&0x01)<<UPSHIFT_4KB) + (0<<UPSHIFT_1KB);
   m_pCHRmemory [ 1 ] = m_CHRROMmemory [ (m_latch0FE>>1) ] + ((m_latch0FE&0x01)<<UPSHIFT_4KB) + (1<<UPSHIFT_1KB);
   m_pCHRmemory [ 2 ] = m_CHRROMmemory [ (m_latch0FE>>1) ] + ((m_latch0FE&0x01)<<UPSHIFT_4KB) + (2<<UPSHIFT_1KB);
   m_pCHRmemory [ 3 ] = m_CHRROMmemory [ (m_latch0FE>>1) ] + ((m_latch0FE&0x01)<<UPSHIFT_4KB) + (3<<UPSHIFT_1KB);
   m_pCHRmemory [ 4 ] = m_CHRROMmemory [ (m_latch1FE>>1) ] + ((m_latch1FE&0x01)<<UPSHIFT_4KB) + (0<<UPSHIFT_1KB);
   m_pCHRmemory [ 5 ] = m_CHRROMmemory [ (m_latch1FE>>1) ] + ((m_latch1FE&0x01)<<UPSHIFT_4KB) + (1<<UPSHIFT_1KB);
   m_pCHRmemory [ 6 ] = m_CHRROMmemory [ (m_latch1FE>>1) ] + ((m_latch1FE&0x01)<<UPSHIFT_4KB) + (2<<UPSHIFT_1KB);
   m_pCHRmemory [ 7 ] = m_CHRROMmemory [ (m_latch1FE>>1) ] + ((m_latch1FE&0x01)<<UPSHIFT_4KB) + (3<<UPSHIFT_1KB);
}

void CROMMapper009::LOAD ( MapperState* data )
{
   CROM::LOAD ( data );
}

void CROMMapper009::SAVE ( MapperState* data )
{
   CROM::SAVE ( data );
}

uint32_t CROMMapper009::MAPPER ( uint32_t addr )
{
   switch ( addr&0xF000 )
   {
      case 0xA000:
         return m_reg [ 0 ];
         break;
      case 0xB000:
         return m_reg [ 1 ];
         break;
      case 0xC000:
         return m_reg [ 2 ];
         break;
      case 0xD000:
         return m_reg [ 3 ];
         break;
      case 0xE000:
         return m_reg [ 4 ];
         break;
      case 0xF000:
         return m_reg [ 5 ];
         break;
   }

   return 0xFF;
}

void CROMMapper009::MAPPER ( uint32_t addr, uint8_t data )
{
   int32_t reg;

   switch ( addr&0xF000 )
   {
      case 0xA000:
         reg = 0;
         m_reg [ 0 ] = data;
         m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ data ];
         m_PRGROMbank [ 0 ] = data;
         break;
      case 0xB000:
         reg = 1;
         m_reg [ 1 ] = data;
         m_latch0FD = data;

         if ( m_latch0 == 0xFD )
         {
            m_pCHRmemory [ 0 ] = m_CHRROMmemory [ (m_latch0FD>>1) ] + ((m_latch0FD&0x01)<<UPSHIFT_4KB) + (0<<UPSHIFT_1KB);
            m_pCHRmemory [ 1 ] = m_CHRROMmemory [ (m_latch0FD>>1) ] + ((m_latch0FD&0x01)<<UPSHIFT_4KB) + (1<<UPSHIFT_1KB);
            m_pCHRmemory [ 2 ] = m_CHRROMmemory [ (m_latch0FD>>1) ] + ((m_latch0FD&0x01)<<UPSHIFT_4KB) + (2<<UPSHIFT_1KB);
            m_pCHRmemory [ 3 ] = m_CHRROMmemory [ (m_latch0FD>>1) ] + ((m_latch0FD&0x01)<<UPSHIFT_4KB) + (3<<UPSHIFT_1KB);
         }

         break;
      case 0xC000:
         reg = 2;
         m_reg [ 2 ] = data;
         m_latch0FE = data;

         if ( m_latch0 == 0xFE )
         {
            m_pCHRmemory [ 0 ] = m_CHRROMmemory [ (m_latch0FE>>1) ] + ((m_latch0FE&0x01)<<UPSHIFT_4KB) + (0<<UPSHIFT_1KB);
            m_pCHRmemory [ 1 ] = m_CHRROMmemory [ (m_latch0FE>>1) ] + ((m_latch0FE&0x01)<<UPSHIFT_4KB) + (1<<UPSHIFT_1KB);
            m_pCHRmemory [ 2 ] = m_CHRROMmemory [ (m_latch0FE>>1) ] + ((m_latch0FE&0x01)<<UPSHIFT_4KB) + (2<<UPSHIFT_1KB);
            m_pCHRmemory [ 3 ] = m_CHRROMmemory [ (m_latch0FE>>1) ] + ((m_latch0FE&0x01)<<UPSHIFT_4KB) + (3<<UPSHIFT_1KB);
         }

         break;
      case 0xD000:
         reg = 3;
         m_reg [ 3 ] = data;
         m_latch1FD = data;

         if ( m_latch1 == 0xFD )
         {
            m_pCHRmemory [ 4 ] = m_CHRROMmemory [ (m_latch1FD>>1) ] + ((m_latch1FD&0x01)<<UPSHIFT_4KB) + (0<<UPSHIFT_1KB);
            m_pCHRmemory [ 5 ] = m_CHRROMmemory [ (m_latch1FD>>1) ] + ((m_latch1FD&0x01)<<UPSHIFT_4KB) + (1<<UPSHIFT_1KB);
            m_pCHRmemory [ 6 ] = m_CHRROMmemory [ (m_latch1FD>>1) ] + ((m_latch1FD&0x01)<<UPSHIFT_4KB) + (2<<UPSHIFT_1KB);
            m_pCHRmemory [ 7 ] = m_CHRROMmemory [ (m_latch1FD>>1) ] + ((m_latch1FD&0x01)<<UPSHIFT_4KB) + (3<<UPSHIFT_1KB);
         }

         break;
      case 0xE000:
         reg = 4;
         m_reg [ 4 ] = data;
         m_latch1FE = data;

         if ( m_latch1 == 0xFE )
         {
            m_pCHRmemory [ 4 ] = m_CHRROMmemory [ (m_latch1FE>>1) ] + ((m_latch1FE&0x01)<<UPSHIFT_4KB) + (0<<UPSHIFT_1KB);
            m_pCHRmemory [ 5 ] = m_CHRROMmemory [ (m_latch1FE>>1) ] + ((m_latch1FE&0x01)<<UPSHIFT_4KB) + (1<<UPSHIFT_1KB);
            m_pCHRmemory [ 6 ] = m_CHRROMmemory [ (m_latch1FE>>1) ] + ((m_latch1FE&0x01)<<UPSHIFT_4KB) + (2<<UPSHIFT_1KB);
            m_pCHRmemory [ 7 ] = m_CHRROMmemory [ (m_latch1FE>>1) ] + ((m_latch1FE&0x01)<<UPSHIFT_4KB) + (3<<UPSHIFT_1KB);
         }

         break;
      case 0xF000:
         reg = 5;
         m_reg [ 5 ] = data;

         if ( !(CPPU::FOURSCREEN()) )
         {
            if ( data&0x01 )
            {
               CPPU::MIRRORHORIZ ();
            }
            else
            {
               CPPU::MIRRORVERT ();
            }
         }

         break;
   }

#if defined ( IDE_BUILD )
   // Check mapper state breakpoints...
   CNES::CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,reg);
#endif
}

void CROMMapper009::LATCH ( uint32_t addr )
{
   if ( (addr&0x1FF0) == 0x0FD0 )
   {
      m_latch0 = 0xFD;
      m_pCHRmemory [ 0 ] = m_CHRROMmemory [ (m_latch0FD>>1) ] + ((m_latch0FD&0x01)<<UPSHIFT_4KB) + (0<<UPSHIFT_1KB);
      m_pCHRmemory [ 1 ] = m_CHRROMmemory [ (m_latch0FD>>1) ] + ((m_latch0FD&0x01)<<UPSHIFT_4KB) + (1<<UPSHIFT_1KB);
      m_pCHRmemory [ 2 ] = m_CHRROMmemory [ (m_latch0FD>>1) ] + ((m_latch0FD&0x01)<<UPSHIFT_4KB) + (2<<UPSHIFT_1KB);
      m_pCHRmemory [ 3 ] = m_CHRROMmemory [ (m_latch0FD>>1) ] + ((m_latch0FD&0x01)<<UPSHIFT_4KB) + (3<<UPSHIFT_1KB);
   }
   else if ( (addr&0x1FF0) == 0x0FE0 )
   {
      m_latch0 = 0xFE;
      m_pCHRmemory [ 0 ] = m_CHRROMmemory [ (m_latch0FE>>1) ] + ((m_latch0FE&0x01)<<UPSHIFT_4KB) + (0<<UPSHIFT_1KB);
      m_pCHRmemory [ 1 ] = m_CHRROMmemory [ (m_latch0FE>>1) ] + ((m_latch0FE&0x01)<<UPSHIFT_4KB) + (1<<UPSHIFT_1KB);
      m_pCHRmemory [ 2 ] = m_CHRROMmemory [ (m_latch0FE>>1) ] + ((m_latch0FE&0x01)<<UPSHIFT_4KB) + (2<<UPSHIFT_1KB);
      m_pCHRmemory [ 3 ] = m_CHRROMmemory [ (m_latch0FE>>1) ] + ((m_latch0FE&0x01)<<UPSHIFT_4KB) + (3<<UPSHIFT_1KB);
   }
   else if ( (addr&0x1FF0) == 0x1FD0 )
   {
      m_latch1 = 0xFD;
      m_pCHRmemory [ 4 ] = m_CHRROMmemory [ (m_latch1FD>>1) ] + ((m_latch1FD&0x01)<<UPSHIFT_4KB) + (0<<UPSHIFT_1KB);
      m_pCHRmemory [ 5 ] = m_CHRROMmemory [ (m_latch1FD>>1) ] + ((m_latch1FD&0x01)<<UPSHIFT_4KB) + (1<<UPSHIFT_1KB);
      m_pCHRmemory [ 6 ] = m_CHRROMmemory [ (m_latch1FD>>1) ] + ((m_latch1FD&0x01)<<UPSHIFT_4KB) + (2<<UPSHIFT_1KB);
      m_pCHRmemory [ 7 ] = m_CHRROMmemory [ (m_latch1FD>>1) ] + ((m_latch1FD&0x01)<<UPSHIFT_4KB) + (3<<UPSHIFT_1KB);
   }
   else if ( (addr&0x1FF0) == 0x1FE0 )
   {
      m_latch1 = 0xFE;
      m_pCHRmemory [ 4 ] = m_CHRROMmemory [ (m_latch1FE>>1) ] + ((m_latch1FE&0x01)<<UPSHIFT_4KB) + (0<<UPSHIFT_1KB);
      m_pCHRmemory [ 5 ] = m_CHRROMmemory [ (m_latch1FE>>1) ] + ((m_latch1FE&0x01)<<UPSHIFT_4KB) + (1<<UPSHIFT_1KB);
      m_pCHRmemory [ 6 ] = m_CHRROMmemory [ (m_latch1FE>>1) ] + ((m_latch1FE&0x01)<<UPSHIFT_4KB) + (2<<UPSHIFT_1KB);
      m_pCHRmemory [ 7 ] = m_CHRROMmemory [ (m_latch1FE>>1) ] + ((m_latch1FE&0x01)<<UPSHIFT_4KB) + (3<<UPSHIFT_1KB);
   }
}