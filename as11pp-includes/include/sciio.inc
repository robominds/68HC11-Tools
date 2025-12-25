#header serio.inc()
*
outbyt   psha
         psha
         bsr    outlhlf
         pula
         bsr    outrhlf
         pula
         rts
*
outlhlf  lsra
         lsra
         lsra
         lsra
outrhlf  anda   #$0f
         adda   #'0
         cmpa   #'9
         ble    outa
         adda   #$07
outa     bsr    outsci2
         rts
outsci2  ldx    #regbas
         brclr  _scsr,x #bit7 *
         staa   _scdr,x
         rts
*
insci    ldx    #regbas
         clra
         brclr  _scsr,x #bit5 inscix
         ldaa   _scdr,x
inscix   rts
*
