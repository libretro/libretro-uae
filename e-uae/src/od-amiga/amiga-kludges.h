/* 
 * sam: GCC has a bad feature ("bug" would a better word) for uae in 
 * libamiga.a: bltsize, bltcon0, ... are defined as absolute address
 * in memory, causing real customchip registers being modified when
 * UAE is running. (I hate this feature... It makes  me lost lots of 
 * time looking for a bug on my side =:-( ).
 *
 * This cures the problem (I hope, since I don't exactly know which 
 * variables need to be redefined for UAE):
 */
/* 0.6.1 */
#define bltsize my_bltsize
#define bltcon0 my_bltcon0
#define bltcon1 my_bltcon1
#define bltapt  my_bltapt
#define bltbpt  my_bltbpt
#define bltcpt  my_bltcpt
#define bltdpt  my_bltdpt
#define adkcon  my_adkcon
/* 0.6.3 */
#define serdat  my_serdat
#define dmacon  my_dmacon
#define intena  my_intena
#define intreq  my_intreq

#define AMIGALIB_NEED_TO_BE_REALLY_SURE
/*
 * Those are here is case of doubt (they are all absolute address
 * in libamiga.a, defining CUSTOM_NEED_TO_BE_REALLY_SURE will prevent
 * uae from using any absolute variable.
 */
#ifdef AMIGALIB_NEED_TO_BE_REALLY_SURE 
#define bootrom my_bootrom
#define cartridge my_cartridge
#define romend my_romend
#define romstart my_romstart
#define adkcon my_adkcon
#define adkconr my_adkconr
#define aud my_aud
#define bltadat my_bltadat
#define bltafwm my_bltafwm
#define bltalwm my_bltalwm
#define bltamod my_bltamod
#define bltapt my_bltapt
#define bltbdat my_bltbdat
#define bltbmod my_bltbmod
#define bltbpt my_bltbpt
#define bltcdat my_bltcdat
#define bltcmod my_bltcmod
#define bltcon0 my_bltcon0
#define bltcon1 my_bltcon1
#define bltcpt my_bltcpt
#define bltddat my_bltddat
#define bltdmod my_bltdmod
#define bltdpt my_bltdpt
#define bltsize my_bltsize
#define bpl1mod my_bpl1mod
#define bpl2mod my_bpl2mod
#define bplcon0 my_bplcon0
#define bplcon1 my_bplcon1
#define bplcon2 my_bplcon2
#define bpldat my_bpldat
#define bplpt my_bplpt
#define clxcon my_clxcon
#define clxdat my_clxdat
/* Oh look ! libamiga.a does not allow you to define a shared
   variable caled color.... This is quite annoying :-/ */
#define color my_color
#define cop1lc my_cop1lc
#define cop2lc my_cop2lc
#define copcon my_copcon
#define copins my_copins
#define copjmp1 my_copjmp1
#define copjmp2 my_copjmp2
//#define custom my_custom
#define ddfstop my_ddfstop
#define ddfstrt my_ddfstrt
#define diwstop my_diwstop
#define diwstrt my_diwstrt
#define dmacon my_dmacon
#define dmaconr my_dmaconr
#define dskbytr my_dskbytr
#define dskdat my_dskdat
#define dskdatr my_dskdatr
#define dsklen my_dsklen
#define dskpt my_dskpt
#define intena my_intena
#define intenar my_intenar
#define intreq my_intreq
#define intreqr my_intreqr
#define joy0dat my_joy0dat
#define joy1dat my_joy1dat
#define joytest my_joytest
#define pot0dat my_pot0dat
#define pot1dat my_pot1dat
#define potgo my_potgo
#define potinp my_potinp
#define refptr my_refptr
#define serdat my_serdat
#define serdatr my_serdatr
#define serper my_serper
#define spr my_spr
#define sprpt my_sprpt
#define vhposr my_vhposr
#define vhposw my_vhposw
#define vposr my_vposr
#define vposw my_vposw
#define ciaa my_ciaa
#define ciaacra my_ciaacra
#define ciaacrb my_ciaacrb
#define ciaaddra my_ciaaddra
#define ciaaddrb my_ciaaddrb
#define ciaaicr my_ciaaicr
#define ciaapra my_ciaapra
#define ciaaprb my_ciaaprb
#define ciaasdr my_ciaasdr
#define ciaatahi my_ciaatahi
#define ciaatalo my_ciaatalo
#define ciaatbhi my_ciaatbhi
#define ciaatblo my_ciaatblo
#define ciaatodhi my_ciaatodhi
#define ciaatodlow my_ciaatodlow
#define ciaatodmid my_ciaatodmid
#define ciab my_ciab
#define ciabcra my_ciabcra
#define ciabcrb my_ciabcrb
#define ciabddra my_ciabddra
#define ciabddrb my_ciabddrb
#define ciabicr my_ciabicr
#define ciabpra my_ciabpra
#define ciabprb my_ciabprb
#define ciabsdr my_ciabsdr
#define ciabtahi my_ciabtahi
#define ciabtalo my_ciabtalo
#define ciabtbhi my_ciabtbhi
#define ciabtblo my_ciabtblo
#define ciabtodhi my_ciabtodhi
#define ciabtodlow my_ciabtodlow
#define ciabtodmid my_ciabtodmid
#endif /* AMIGALIB_NEED_TO_BE_REALLY_SURE */
