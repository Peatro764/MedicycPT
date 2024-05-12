C-------------------------------------------------------------------------------
C	creation de compensateurs spheriques  		juin 96
C					revision	avril 98,sept 01,juin 02
C	J_HERAULT
C-------------------------------------------------------------------------------
	DIMENSION C1X(500),C1Y1(500),C1Y2(500),C2X(500),C2Y1(500),C2Y2(500)
	DIMENSION XXXX(50),YYYY(50),X(50),Y(50),XN(50),YN(50),X_BOLUS(50)
	DIMENSION Y_BOLUS(50),XP_BOLUS(50),YP_BOLUS(50),XPN_BOLUS(50)
	DIMENSION YPN_BOLUS(50),XP(50),YP(50),XD_BOLUS(50),YD_BOLUS(50)
	DIMENSION XDN_BOLUS(50),YDN_BOLUS(50),MM(20),II(20)
	LOGICAL VERI,TEST,CONDI,VERIF,BOUL
	INTEGER II,MM
	CHARACTER*5 XP,YP,PATIENT*50,DATEJ*10,TIMEJ*10,DATI*20,NUMDOS*20
	CHARACTER*15 TE_SCLERE,TE_PAUP,TDIAM,TPARM,TXOOO,TYOOO
	CHARACTER*4 TEXT,FICHN*8
	PARAMETER (PI=3.14)
C-------------------------------------------------------------------------------
C	initialisations
C-------------------------------------------------------------------------------
	OPEN (UNIT=20,FILE='[JOEL.ALPHA.DOSIME]BOLUS_SPH.IPT',STATUS='OLD')
	READ (20,30) PATIENT,NUMDOS,DIAM,E_SCLERE,E_PAUP,PARM,XOOO,YOOO
   30	FORMAT(///,A50,//,A20,//,F5.1,//,F5.1,//,F5.1,//,F5.1,///,F5.1,//,F5.1)
	EP_TT=PARM-E_PAUP
	EP_BOLUS=EP_TT-E_SCLERE
	I_DIAM_OUT=INT(DIAM*10.)+1
	I_SHIFT_OUT=I_DIAM_OUT/2
	RAYON_OUT=DIAM/2.
	I_DIAM_IN=INT((DIAM-2*E_SCLERE)*10.)+1
	I_SHIFT_IN=I_DIAM_IN/2
	RAYON_IN=DIAM/2.-E_SCLERE
 	I_RAYON_IN=INT(RAYON_IN)
C-------------------------------------------------------------------------------
C	calcul de la geometrie de l'oeil
C-------------------------------------------------------------------------------
	C1X(1)=-RAYON_OUT
	C1Y1(1)=RAYON_OUT
	C1Y2(1)=RAYON_OUT
	DO I=2,I_DIAM_OUT
		C1X(I)=FLOAT(I-I_SHIFT_OUT)/10.
		AGEN=C1X(I)-RAYON_OUT
		IF (AGEN.GE.0.0) THEN
			C1X(I)=RAYON_OUT
			C1Y1(I)=0.
		ELSE
			C1Y1(I)=(RAYON_OUT**2-C1X(I)**2)**0.5
		END IF
		C1Y2(I)=-C1Y1(I)
		C1Y1(I)=C1Y1(I)+RAYON_OUT
		C1Y2(I)=C1Y2(I)+RAYON_OUT
	END DO
	C2X(1)=-RAYON_IN
	C2Y1(1)=RAYON_OUT
	C2Y2(1)=RAYON_OUT
 	DO I=2,I_DIAM_IN
		C2X(I)=FLOAT(I-I_SHIFT_IN)/10.
		AGEN=C2X(I)-RAYON_IN
		IF (AGEN.GE.0.0) THEN
			C2X(I)=RAYON_IN
			C2Y1(I)=0.
		ELSE
			C2Y1(I)=(RAYON_IN**2-C2X(I)**2)**0.5
		END IF
		C2Y2(I)=-C2Y1(I)
		C2Y1(I)=C2Y1(I)+RAYON_OUT
		C2Y2(I)=C2Y2(I)+RAYON_OUT
	END DO
	TEST=.FALSE.
	DO I=1,I_RAYON_IN
		XXXX(I)=FLOAT(I)
		VERI=.FALSE.
		DO K=I_SHIFT_IN,I_DIAM_IN
			IF ((C2X(K).GE.XXXX(I)).AND.(VERI.EQ..FALSE.)) THEN
				SUB=(C2X(K)-C2X(K-1))
				IF (SUB.EQ.0.) THEN
					YYYY(I)=C2Y2(K-1)
				ELSE
					MULTI=(C2Y2(K)-C2Y2(K-1))/SUB
					YYYY(I)=C2Y2(K-1)+MULTI*(C2X(K-1)-XXXX(I))
					VERI=.TRUE.
				END IF
			END IF
		END DO
	END DO	
	X(1)=0.
	Y(1)=YYYY(1)
	K=2
	DO I=1,I_RAYON_IN-1
		X(K)=XXXX(I)
		Y(K)=YYYY(I)
		X(K+1)=X(K)
      		Y(K+1)=YYYY(I+1)	
		K=K+2	
	END DO
	K=K-1
C-------------------------------------------------------------------------------
C	conditions de fermeture
C-------------------------------------------------------------------------------
	DIFF=RAYON_IN-XXXX(I_RAYON_IN)
	IF (DIFF.GE.0.0) THEN 
		VERIF=.TRUE.
		KMAX=K+4
		YINF=Y(K-1)
		YSUP=Y(K)
		XINF=X(K)
	ELSE
		VERIF=.FALSE.
		KMAX=K+2
	END IF
	IF (VERIF.EQ..TRUE.) THEN
                X(KMAX)=RAYON_IN
               	X(KMAX-1)=X(KMAX)
               	X(KMAX-2)=RAYON_IN-(RAYON_IN-XINF)/2.
		X(KMAX-3)=X(KMAX-2)
		Y(KMAX)=RAYON_OUT
 		Y(KMAX-3)=RAYON_OUT-(RAYON_IN**2-X(KMAX-2)**2)**0.5
                Y(KMAX-1)=RAYON_OUT-(RAYON_OUT-Y(KMAX-3))/2.
                Y(KMAX-2)=Y(KMAX-1)
 		Y(KMAX-4)= Y(KMAX-3)
	ELSE
		X(KMAX-1)=RAYON_IN
		Y(KMAX-1)=Y(KMAX-2)
		X(KMAX)=X(KMAX-1)
		Y(KMAX)=RAYON_OUT
	END IF
 	DO K=1,KMAX
		XN(K)=-X(K)
		YN(K)=Y(K)
	END DO
	CONDI=.FALSE.
	DO J=1,KMAX
		Y_BOLUS(J)=EP_BOLUS-(Y(J)-E_SCLERE)
		X_BOLUS(J)=X(J)
		IF ((Y_BOLUS(J).LT.0.0).AND.(CONDI.EQ..FALSE.)) THEN
			CONDI=.TRUE.
			JJ_MAX=J
		END IF
 	END DO		
	IF (CONDI.EQ..FALSE.) JJ_MAX=KMAX
	Y_BOLUS(JJ_MAX)=0.
	Y_P_DIF=EP_TT-RAYON_OUT
	YUP=DIAM-E_SCLERE-RAYON_OUT
	YDO=E_SCLERE-RAYON_OUT
	IF ((Y_P_DIF.GT.YDO).AND.(Y_P_DIF.LT.YUP)) THEN 
		X_P_DIF=(RAYON_IN**2-Y_P_DIF**2)**0.5
	 	XXL=X_P_DIF
	ELSE
		WRITE (*,*) 'ERREUR D''INITIALISATION RECOMMENCEZ L''OPERATION'
		GOTO 10
	END IF
	BOUL=.FALSE.
	DO I=1,JJ_MAX
		X_DIFF=X_BOLUS(I)-XXL
		IF ((X_DIFF.GT.0.0).AND.(BOUL.EQ..FALSE.)) THEN 
			KK_MAX=I+1
			X_BOLUS(I)=XXL
			X_BOLUS(I+1)=XXL
			Y_BOLUS(I+1)=0.
			BOUL=.TRUE.
		END IF	
	END DO
	IF (BOUL.EQ..TRUE.) JJ_MAX=KK_MAX
C-------------------------------------------------------------------------------
C	conversion du compensateur en epaisseur plexiglas
C-------------------------------------------------------------------------------
	DO J=1,JJ_MAX
		XP_BOLUS(J)=X_BOLUS(J)
		YP_BOLUS(J)=Y_BOLUS(J)*1.05/1.18	
		XPN_BOLUS(J)=-XP_BOLUS(J)
		YPN_BOLUS(J)=YP_BOLUS(J)	
		WRITE(XP(J),20) XP_BOLUS(J)
		WRITE(YP(J),20) YP_BOLUS(J)
	END DO
	COUPE=YP_BOLUS(JJ_MAX-1)-YP_BOLUS(JJ_MAX)-0.15 
	IF (COUPE.LT.0.0) THEN 
		YP_BOLUS(JJ_MAX-2)=0.
		YPN_BOLUS(JJ_MAX-2)=0.
		JJ_MAX=JJ_MAX-2
	END IF
  20	FORMAT (F5.1)
C-------------------------------------------------------------------------------
C	deformation des isodoses
C-------------------------------------------------------------------------------
	DO J=1,JJ_MAX
		XD_BOLUS(J)=X_BOLUS(J)
		YD_BOLUS(J)=PARM-E_PAUP-Y_BOLUS(J)	
		XDN_BOLUS(J)=-XD_BOLUS(J)
		YDN_BOLUS(J)=YD_BOLUS(J)	
	END DO
	JD_MAX=JJ_MAX+1
	XD_BOLUS(JD_MAX)=15.
	XDN_BOLUS(JD_MAX)=-15.
	YD_BOLUS(JD_MAX)=YD_BOLUS(JJ_MAX)
	YDN_BOLUS(JD_MAX)=YDN_BOLUS(JJ_MAX)	
C-------------------------------------------------------------------------------
C	graphiques
C-------------------------------------------------------------------------------
	L=999999
	F=1E36
	CALL DATE (DATEJ)
	CALL TIME (TIMEJ)
	DATI=DATEJ//TIMEJ
      	CALL GRAOPE
	CALL GRABSX (200.,200.,700.,500.,'Compensateur spherique')
	CALL GRAVWP(1.,14.5,3.,3.)
	CALL GRAWND(-15.,15.,0.,30.)
	CALL GRAXI1 (-4,L,L,F)                                        
	CALL GRAYI1 (-4,L,L,F)                                        
	CALL GRAHED ('Representation de ',0.3)
	CALL GRAHED ('l''oeil ECH:1',0.3)
	CALL GRACOM (.3,'X(mm)',L,'Y(mm)',1)
 	CALL GRAPH1 (C1X,C1Y1,I_DIAM_OUT,1,5,0.0)
	CALL GRAPH1 (C1X,C1Y2,I_DIAM_OUT,1,5,0.0)
	CALL GRAPH1 (C2X,C2Y1,I_DIAM_IN,1,5,0.0)
	CALL GRAPH1 (C2X,C2Y2,I_DIAM_IN,1,5,0.0)
 	CALL GRAPH1 (X,Y,KMAX,1,5,0.0)
	CALL GRAPH1 (XN,YN,KMAX,1,5,0.0)
	CALL GRACLX(0,0,0,1)                                         
      	CALL GRAOPE
	CALL GRAVWP(1.,1.5,3.6,3.6)
	CALL GRAWND(-15.,15.,0.,30.)
	CALL GRAXI1 (-4,L,L,F)                                        
	CALL GRAYI1 (-4,L,L,F)                                        
	CALL GRAHED ('Representation du',0.3)
	CALL GRAHED ('compensateur Ech:1.2',0.3)
	CALL GRACOM (.3,'X(mm)',L,'Y(mm)',1)
	CALL GRAPH1 (XP_BOLUS,YP_BOLUS,JJ_MAX,1,5,0.0)
	CALL GRAPH1 (XPN_BOLUS,YPN_BOLUS,JJ_MAX,1,5,0.0)
	X5=8.0
	X6=8.5
	X7=9.0
	X8=11.
	CALL GRATEX (X5,16.,.35,'Coordonnees du compensateur',0.,XE,YE,1)
	CALL GRATEX (X6,15.5,.35,'[Y]=mm plexiglas',0.,XE,YE,1)
	CALL GRATEX (X7,14.5,.35,'X',0.,XE,YE,1)
	CALL GRATEX (X8,14.5,.35,'Y',0.,XE,YE,1)
	YINC=14.
	DO I=1,JJ_MAX
		CALL GRATEX (X7,YINC,.35,XP(I),0.,XE,YE,1)
		CALL GRATEX (X8,YINC,.35,YP(I),0.,XE,YE,1)
		YINC=YINC-0.5
	END DO
	WRITE(TDIAM,20) DIAM
 	WRITE(TE_SCLERE,20) E_SCLERE
 	WRITE(TE_PAUP,20) E_PAUP
 	WRITE(TPARM,20) PARM
	WRITE(TXOOO,20) XOOO
	WRITE(TYOOO,20) YOOO
	TDIAM='D_OEIL='//TDIAM
	TE_SCLERE='E_SCLERE='//TE_SCLERE
	TE_PAUP='E_PAUP='//TE_PAUP
	TPARM='PAR_MAX='//TPARM
	TXOOO='XO='//TXOOO
	TYOOO='YO='//TYOOO
	X1=15.
	X2=18.
	X3=.4
	X4=.3
	X20=22.
	X21=22.
  	CALL GRATEX (X1,2.,X3,PATIENT,0.,XE,YE,1)
  	CALL GRATEX (X20,2.,X3,NUMDOS,0.,XE,YE,1)
  	CALL GRATEX (X1,1.5,X3,DATI,0.,XE,YE,1)
  	CALL GRATEX (X1,1.,X4,'Rappel des donnees (mm)',0.,XE,YE,1)
 	CALL GRATEX (X1,.5,X4,TDIAM,0.,XE,YE,1)
	CALL GRATEX (X2,.5,X4,TE_SCLERE,0.,XE,YE,1)
  	CALL GRATEX (X1,0.,X4,TE_PAUP,0.,XE,YE,1)
  	CALL GRATEX (X2,0.,X4,TPARM,0.,XE,YE,1)
  	CALL GRATEX (X21,0.5,X4,TXOOO,0.,XE,YE,1)
  	CALL GRATEX (X21,0.,X4,TYOOO,0.,XE,YE,1)
  	CALL GRACLX(0,0,0,1)                                        
      	CALL GRAOPE
	CALL GRAVWP(1.,8.,3.6,3.6)
	CALL GRAWND(-15.,15.,0.,30.)
	CALL GRAYI1 (-6,L,L,0.)                                        
	CALL GRAXI1 (-6,L,L,XP_BOLUS(JJ_MAX))
	CALL GRAHED ('Centrage Ech:1.2 ',0.3)
	CALL GRAHED ('du compensateur ',0.3)
	CALL GRACOM (.2,' ',L,' ',1)
	DO I= 1,I_DIAM_OUT
		C1X(I)=C1X(I)* XP_BOLUS(JJ_MAX)/RAYON_OUT
		C1Y1(I)=C1Y1(I)* XP_BOLUS(JJ_MAX)/RAYON_OUT
		C1Y2(I)=C1Y2(I)* XP_BOLUS(JJ_MAX)/RAYON_OUT
	END DO
 	CALL GRAPH1 (C1X,C1Y1,I_DIAM_OUT,1,5,0.0)
	CALL GRAPH1 (C1X,C1Y2,I_DIAM_OUT,1,5,0.0)
	CALL GRACLX(0,0,0,1)                                         
      	CALL GRAOPE
	CALL GRAVWP(15.,4.5,12.,12.)
	CALL GRAWND(-15.,15.,0.,30.)
	CALL GRAXI1 (-6,L,L,0.)                                        
	CALL GRAYI1 (-6,L,L,0.)                                        
	CALL GRAHED ('Deformation a l''echelle 4 des isodoses dans',0.4)
	CALL GRAHED ('un plan passant par le centre de l''oeil',0.4)
	CALL GRAHED ('L''axe des abscisses est le plan corneen ',0.4)
	CALL GRACOM (X3,'X(mm)',L,'Y(mm)',1)
	CALL GRAPH1 (XD_BOLUS,YD_BOLUS,JD_MAX,1,5,0.0)
	CALL GRAPH1 (XDN_BOLUS,YDN_BOLUS,JD_MAX,1,5,0.0)
	CALL GRACLS                                         
  10	CONTINUE
C
C-------------------------------------------------------------------------------
C	creation du programme pour la fraiseuse
C-------------------------------------------------------------------------------
	TEXT=NUMDOS(3:6)
	FICHN=TEXT//'.DAT'
  	OPEN (UNIT=7,FILE='[MEDICAL.PROTON.MODUL.PATI]'//FICHN,TYPE='NEW',FORM='FORMATTED')
	WRITE (7,200) TEXT
 200	FORMAT (X,'0 BEGIN PGM ',A4,' MM')
	WRITE (7,201)
 201	FORMAT (X,'1 BLK FORM 0.1 Z X-20 Y-20 Z-1'/
	1	X,'2 BLK FORM 0.2 X+20 Y+20 Z+27.5'/   
	2	X,'3 TOOL DEF 1 L+0 R+6'/
	3	X,'4 TOOL DEF 2 L+0 R+2.5'/
	4	X,'5 TOOL DEF 3 L-11.5 R+1.5'/
	5	X,'6 TOOL CALL 1 Z S 2000'/
	6	X,'7 L X+0 Y+0 R0 F MAX M13'/
	7	X,'8 CC X+0 Y+0'/
	8	X,'9 CYCL DEF 5.0 POCHE CIRC.'/
	9	X,'10 CYCL DEF 5.1 DIST. -2'/
	1	X,'11 CYCL DEF 5.2 PROF. -11'/
	2	X,'12 CYCL DEF 5.3 PASSE -5.5 F80'/
	3	X,'13 CYCL DEF 5.4 RAYON 19'/
	4	X,'14 CYCL DEF 5.5 F100 DR+'/
	5	X,'15 L Z+28.5 R0 F MAX M'/
	6	X,'16 CYCL CALL M'/
	7	X,'17 L Z+25 R0 F MAX M'/
	8	X,'18 LP PR+20 PA+0 RL F100 M'/
	9	X,'19 RND R2 F'/
	1	X,'20 CP IPA+360 DR+ RL F M'/
	2	X,'21 RND R2 F'/
	3	X,'22 L X+0 Y+0 R0 F MAX M'/
	4	X,'23 L R F M25'/
	5	X,'24 TOOL CALL 2 Z S 3000'/
	6	X,'25 L X+0 Y+0 R0 F MAX M13'/
	7	X,'26 L Z+25 R0 F MAX M')
	HXO=4
	HYO=4                                                                                   
	IF ((ABS(XOOO).LT.9.95).AND.(XOOO.GE.0.)) HXO=3
	IF ((ABS(YOOO).LT.9.95).AND.(YOOO.GE.0.)) HYO=3
	WRITE (7,209) XOOO,YOOO
 209	FORMAT (X,'27 FN 0: Q1 = ',F<HXO>.1/
	1	X,'28 FN 0: Q2 = ',F<HYO>.1/
	2	X,'29 LBL 3'/
	3	X,'30 L X+Q1 Y+Q2 R0 F MAX M13'/
	4	X,'31 CC X+Q1 Y+Q2'/
	5	X,'32 L Z+20 R0 F MAX M')
	HOBX=4
	HOBY=4
	HOBY3=4
	IF (XP_BOLUS(3).LT.9.95) HOBX=3 
	IF (YP_BOLUS(1).LT.9.95) HOBY=3 
	IF (YP_BOLUS(3).LT.9.95) HOBY3=3 
	WRITE (7,202) YP_BOLUS(1),YP_BOLUS(3),XP_BOLUS(3)
 202	FORMAT (X,'33 FN 0: Q3 = ',F<HOBY>.1/
	1	X,'34 L Z+Q3 R0 F80 M'/
	2	X,'35 FN 0: Q3 = ',F<HOBY3>.1/
	3	X,'36 FN 0: Q4 = ',F<HOBX>.1/
	4	X,'37 LBL 1'/
	5	X,'38 FN 1: Q5 = + Q4 + +3.5'/
	6	X,'39 LP PR+Q5 PA+0 R0 F80 M'/
	7	X,'40 L Z+Q3 R F M'/
	8	X,'41 LP PR+Q4 PA+0 RL F M'/
	9	X,'42 RND R0.5 F'/
	1	X,'43 CP IPA+360 DR- RL F M'/
	2	X,'44 RND R0.5 F'/
	3	X,'45 LP PR+Q5 PA+0 R0 F M'/
	4	X,'46 LBL 0')
	IIK=47
	DO I=5,JJ_MAX,2
	HOBX=4
	HOBY=4
	IF (XP_BOLUS(I).LT.9.95) HOBX=3 
	IF (YP_BOLUS(I).LT.9.95) HOBY=3 
	MM1=2
	IIK1=IIK
	IIK2=IIK+1
	IIK3=IIK+2
	WRITE (7,207) IIK1,YP_BOLUS(I),IIK2,XP_BOLUS(I),IIK3
 207	FORMAT (X,I<MM1>,' FN 0: Q3 = ',F<HOBY>.1/
	1	X,I<MM1>,' FN 0: Q4 = ',F<HOBX>.1/
	2	X,I<MM1>,' CALL LBL 1 REP')
        IIK=IIK+3
        END DO
	II(1)=IIK
	DO I=2,20
		II(I)=II(I-1)+1
		MM(I)=2
		IF (II(I).GE.100) MM(I)=3
	END DO
	WRITE (7,208) (II(I), I=1,20),TEXT
 208	FORMAT (X,I<MM(1)>,' FN 11: IF +Q1 GT +1.5 GOTO LBL 2'/
	1	X,I<MM(2)>,' FN 11: IF +Q2 GT +1.5 GOTO LBL 2'/
	2	X,I<MM(3)>,' FN 12: IF +Q1 LT -1.5 GOTO LBL 2'/
	3	X,I<MM(4)>,' FN 12: IF +Q2 LT -1.5 GOTO LBL 2'/
	4	X,I<MM(5)>,' CC X+0 Y+0'/
	5	X,I<MM(6)>,' LP PR+16.5 PA+0 R0 F M'/
	6	X,I<MM(7)>,' CP IPA+360 DR+ R0 F M'/
	7	X,I<MM(8)>,' LBL 2'/
	8	X,I<MM(9)>,' L Z+30 R0 F MAX M'/
	9	X,I<MM(10)>,' CALL LBL 3 REP 1 /1'/
	1	X,I<MM(11)>,' L R F M25'/
	2	X,I<MM(12)>,' TOOL CALL 3 Z S 3000'/
	3	X,I<MM(13)>,' CC X+0 Y+0'/
	4	X,I<MM(14)>,' LP PR+24.5 PA+135 R0 F MAX M13'/
	5	X,I<MM(15)>,' L Z+30 R0 F MAX M'/
	6	X,I<MM(16)>,' L Z+22 R0 F100 M'/
	7	X,I<MM(17)>,' LP PR+17.5 PA+135 R0 F M'/
	8	X,I<MM(18)>,' L Z+30 R0 F MAX M'/
	9	X,I<MM(19)>,' L R F M25'/
	1	X,I<MM(20)>,' END PGM ',A4,' MM')
	END  
