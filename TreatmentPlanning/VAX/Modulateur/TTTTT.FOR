C-------------------------------------------------------------------------------
C		PROGRAMME DE CALCUL DES MODULATEURS
C
C                       			J.HERAULT 10.10.91	 
C------------------------------------------------------------------------------


C-------------------------------------------------------------------------------
C	ENTREE DU TYPE DES DONNEES
C-------------------------------------------------------------------------------

	CHARACTER*10 DA,HEU,OC,OAC,OAD,OO,DAT*20,NUME*30,NUMERO*50
	CHARACTER*5 V*2,C,TEX1,TEX2,TEX3,TEX4
	INTEGER*4 IND,I,J,W,JMA,IJ,JMA1,KI,KL,COM
	INTEGER*4 IPMAX,IPMIN,ZER,VAR,COU,JMAX,IMAX,JK,II
	LOGICAL BOO
	REAL*4 MAX,SUM,SUM1,SUM2,SUM3,EPS,ZE,SUM4,BN,CJ,YO,PARMAX
	REAL*4 MODULATION,PARCOURS,PASMODUL,DEGRADEUR,WI
	REAL*4 PARJ,XXX,MAX1,PARCOURST,MODULATIONT,CORRFIT
	REAL*8 XHER,HER
	DIMENSION P(150),O(150),OA(150),OB(150),A(150,150),FI(150)
	DIMENSION TEST(150),SUM3(150),BN(150),C(150),V(150)
	DIMENSION OC(150),OAC(150),OAD(150),B(150,150),OO(150)
	PARAMETER (EPS=0.00001)
	PARAMETER (ZE=0.0)
	PARAMETER (ZER=0)
	PARAMETER (JMAX=149)
	PARAMETER (PARJ=0.2)
	PARAMETER (PARMAX=27.5)
	PARAMETER (PASMODUL=0.8)
C-------------------------------------------------------------------------------
C	INITIALISATIONS
C-------------------------------------------------------------------------------

	WRITE (*,*) 'ENTRER LE NOM ET LE NUMERO DE DOSSIER DU PATIENT '
	READ (*,99) NUME
 99     FORMAT (A30)
	WRITE (*,*) 'ENTRER LE PARCOURS SOUHAITE (MM DE TISSU OCULAIRE)'
	READ (*,*) PARCOURST
	WRITE (*,*) 'ENTRER LA MODULATION SOUHAITEE (MM DE TISSU OCULAIRE)'
	READ (*,*) MODULATIONT
	PARCOURS=PARCOURST*1.05/1.18
	MODULATION=MODULATIONT*1.05/1.18
	IMAX=JNINT(PARMAX/PASMODUL)
	DO I=1,IMAX
		P(I)=0.
		O(I)=0.
		OA(I)=0.0
		OB(I)=0.0
		DO J=1,JMAX
			A(I,J)=0.
			B(I,J)=0.
		END DO
	END DO       
	DO J=1,JMAX
		SUM3(J)=0.
	END DO

C-------------------------------------------------------------------------------
C	ENTREE DES FICHIERS DE DONNEES PICS DE BRAGG
C-------------------------------------------------------------------------------

	 OPEN (UNIT=20,FILE='[JOEL.BRAGG.DABRAG]BR_FEV92.DAT',STATUS='OLD')	
	 READ (20,20) FI
 20	 FORMAT (F7.3)
	 DO J=1,JMAX
	 	B(1,J)=FI(J)
         END DO

C-------------------------------------------------------------------------------
C	GENERATION DE L''ENSEMBLE DES FICHIERS POSSIBLES
C-------------------------------------------------------------------------------

        DO I=2,IMAX
		XXX=FLOATJ(I-1)
		HER=DPROD(XXX,PASMODUL)
		WI=FONCT(HER)
		DO J=1,JMAX
			JK=J+JINT((I-1)*PASMODUL/PARJ) 
		 	IF (JK.LE.JMAX) THEN
				B(I,J)=B(1,JK)*WI
			ELSE 
				B(I,J)=0.0
			END IF
		END DO
	END DO
	DEGRADEUR=PARMAX-PARCOURS-0.4  !Decalage 90/100% bord distal
	IPMAX=JNINT((DEGRADEUR+MODULATION)/PASMODUL)+1
	IPMIN=JNINT(DEGRADEUR/PASMODUL)
	IF (IPMIN.EQ.0) THEN
		IPMIN=IPMIN+1
		IPMAX=IPMAX+1
	END IF
	IF (IPMAX.GT.JINT(PARMAX/PASMODUL)) THEN
		IPMIN=IPMIN-1
		IPMAX=IPMAX-1
	END IF

C-------------------------------------------------------------------------------
C	GENERATION DE LA MATRICE UTILE
C-------------------------------------------------------------------------------

	DO I=IPMIN,IPMAX
		O(I)=1.
		P(I)=1.
		DO J=1,JMAX
			A(I,J)=B(I,J)	
	        END DO
 	END DO

C-------------------------------------------------------------------------------
C	DEROULEMENT DU PROGRAMME PRINCIPAL
C-------------------------------------------------------------------------------
	
C-------------------------------------------------------------------------------
C	SOMMATION DES PICS DE BRAGG ET RECHERCHE DU POIDS OPTIMAL
C-------------------------------------------------------------------------------

	BOO=.FALSE.
	COU=1
	DO WHILE (BOO.EQ..FALSE.)
        	DO I=IPMIN,IPMAX
	  		IF (COU.EQ.1) THEN 
	   			VAR=I
	  		ELSE
	   			VAR=IPMAX
	  		END IF
	  		IF (P(I).EQ.0.0) GO TO 40
	  		MAX=A(I,1)
	  		JMA=1
	  		DO J=1,JMAX
  				IF (A(I,J).GT.MAX) THEN 
					MAX=A(I,J) 
          				JMA=J
   				END IF
 			END DO 
	  		SUM=0.0
			IF (MAX.EQ.0.0) THEN
				P(I)=0.0
				GO TO 40
			END IF
	  		DO IJ=IPMIN,VAR
	   			SUM=SUM+A(IJ,JMA)
			END DO 
			CORRFIT=1.230
			IF (I.EQ.IPMIN) CORRFIT=1.
			IF (I.EQ.(IPMIN+1)) CORRFIT=1.110
			IF (I.EQ.(IPMIN+2)) CORRFIT=1.160
			IF (I.EQ.(IPMIN+3)) CORRFIT=1.180
			IF (I.EQ.(IPMIN+4)) CORRFIT=1.200
		       	IF (I.EQ.(IPMIN+5)) CORRFIT=1.210
		       	IF (I.EQ.IPMAX) CORRFIT=1.210
		       	IF (I.EQ.IPMIN) THEN
	   			SUM1=SUM
	   			JMA1=JMA
           		ELSE
	  			P(I)=(SUM1*CORRFIT-SUM+MAX)/MAX
	   			IF (P(I).LT.ZE) THEN 
	    				P(I)=0.0
	   			END IF
	  		END IF
 40      		CONTINUE 
	  		TEST(I)=O(I)
	  		O(I)=O(I)*P(I)
	  		DO J=1,JMAX
	   			A(I,J)=P(I)*A(I,J)
	  		END DO
      	  		SUM1=0.0
	  		DO KL=1,VAR
	   			SUM1=SUM1+A(KL,JMA1)
			END DO
      		END DO
	 	COM=0
	 	DO I=IPMIN,IPMAX
	  		X=ABS(TEST(I)-O(I))
          		IF (X.GT.EPS) THEN 
	   			COM=COM+1
     	  		END IF
	 	END DO
	 	IF (COM.EQ.ZER)  THEN
	  		BOO=.TRUE.
	 	END IF
	 	COU=COU+1
 	END DO
	
C-------------------------------------------------------------------------------
C	CALCUL DE LA PONDERATION ASSOCIEE A CHAQUE EPAISSEUR I DE 
C	CONVERTISSEUR D'ENERGIE
C-------------------------------------------------------------------------------
	O(IPMIN)=O(IPMIN)
	O(IPMIN+1)=O(IPMIN+1)
	O(IPMAX)=O(IPMAX)
	DO J=1,JMAX
		A(IPMIN,J)=A(IPMIN,J)
		A(IPMIN+1,J)=A(IPMIN+1,J)
		A(IPMAX,J)=A(IPMAX,J)
	END DO
	MAX1=SUM3(1)
	DO J=1,JMAX
         	DO I=IPMIN,IPMAX
          		SUM3(J)=SUM3(J)+A(I,J)
         	END DO
	 	IF (SUM3(J).GT.MAX1) THEN 
	  		MAX1=SUM3(J)
	 	END IF
	END DO

	DO J=1,JMAX
	 	SUM3(J)=SUM3(J)/MAX1
	END DO

C-------------------------------------------------------------------------------
C	CALCUL DE L'ETENDUE DES SECTEURS ANGULAIRES ASSOCIEE A CHAQUE
C	EPAISSEUR I DE CONVERTISSEUR D'ENERGIE POUR UN MODULATEUR A
C	4 PALES
C-------------------------------------------------------------------------------

	SUM4=0.0
	DO I=IPMIN,IPMAX
	 	SUM4=SUM4+O(I)
	END DO
	DO I=IPMIN,IPMAX
	 	OA(I)=(90.0*O(I))/SUM4
	END DO
	OB(IPMIN)=90.
	DO I=IPMIN+1,IPMAX
		OB(I)=OB(I-1)-OA(I-1)
	END DO
	DO I=IPMIN,IPMAX
		OB(I)=OB(I)/2
		II=I-IPMIN
		WRITE (OO(I),90) II
      	 	WRITE (OC(I),80) O(I)
	 	WRITE (OAC(I),80) OA(I)
		WRITE (OAD(I),80) OB(I)
  80	 	FORMAT (F7.3)
  90		FORMAT (I4)
	END DO

C-------------------------------------------------------------------------------
C	REPRESENTATION GRAPHIQUE DE L'EFFET DE LA PONDERATION
C-------------------------------------------------------------------------------

	L=999999
	F=1E36
	DO J=1,JMAX
		BN(J)=FLOAT(J)*PARJ
	END DO
	CJ=FLOAT(JMAX)*PARJ+PASMODUL
	CALL DATE (DA)
	CALL TIME (HEU)
	DAT=DA//HEU
	NUMERO=DAT//NUME
	WRITE (TEX1,95) DEGRADEUR
	WRITE (TEX2,100) PARCOURST
	WRITE (TEX3,100) MODULATIONT
	WRITE (TEX4,110) PASMODUL
  95	FORMAT (F4.1)
 100    FORMAT (F4.1)
 110	FORMAT (F4.2)
	CALL GRAOPE
	CALL GRACOI (4)
	CALL GRAHED ('SOMMATION DES PICS DE BRAGG',0.5)
	CALL GRAHED ('y = #2#S#1#o(i)a(i,j)',.5)
	CALL GRAHED (NUMERO,.5)
	CALL GRAVWP (F,F,20.,15.)
	CALL GRAWND (0.,CJ,0.,1.2)
	CALL GRACOM (.5,'Epaisseur de plexiglass (mm)',L,'y',.5)
	CALL GRAXI1 (-4,L,L,F)
        CALL GRAYI1 (-4,L,L,F)
	CALL GRAPH1 (BN,SUM3,JMAX,1,5,0.0)
	CALL GRATEX (3.,5.5,.35,'Unites en mm de plexiglass',0.,XE,YE,1)
	CALL GRATEX (3.,5.,.35,'Degradeur='//TEX1,0.,XE,YE,1)
	CALL GRATEX (3.,4.5,.35,'Pas de la modulation='//TEX4,0.,XE,YE,1)
        CALL GRATEX (10.,5.5,.35,'Unites en mm de tissu oculaire',0.,
     1	XE,YE,1)
	CALL GRATEX (10.,5.,.35,'Parcours demande='//TEX2,0.,XE,YE,1)
	CALL GRATEX (10.,4.5,.35,'Modulation demandee='//TEX3,0.,XE,YE,1)
	CALL GRATEX (22.,23.,.35,'Pas',0.,XE,YE,1)
	CALL GRATEX (25.,23.,.35,'O(i)',0.,XE,YE,1)
	CALL GRATEX (28.,23.,.35,'OA(i)',0.,XE,YE,1)
	CALL GRATEX (31.,23.,.35,'OB(i)',0.,XE,YE,1)
	YO=23.
	DO I=IPMIN,IPMAX
		YO=YO-.5
		CALL GRATEX (22.,YO,.35,OO(I),0.,XE,YE,1)
	 	CALL GRATEX (25.,YO,.35,OC(I),0.,XE,YE,1)
	 	CALL GRATEX (28.,YO,.35,OAC(I),0.,XE,YE,1)
		CALL GRATEX (31.,YO,.35,OAD(I),0.,XE,YE,1)
	END DO
	DO I=1,1
		CALL GRALST(1,0,1)
	END DO
	CALL GRASTP
C-------------------------------------------------------------------------------
C	SI LE RESULTAT N'EST PAS SATISFAISANT RECOMMENCER L'OPERATION
C	UN PAS PLUS GRAND OU PLUS PETIT.
C-------------------------------------------------------------------------------
	
   	END

C-------------------------------------------------------------------------------
C	ENTREE DE LA FONCTION POLYNOMIALE RESULTANT DE L'AJUSTEMENT 
C	DE LA VARIATION DU MAXIMUM DES COURBES DE BRAGG.
C-------------------------------------------------------------------------------

        FUNCTION FONCT(XHER)
	FONCT=1.0084707-1.2231932E-1*XHER+7.0574773E-3*XHER**2
     1	-2.0157541E-4*XHER**3+2.1806800E-6*XHER**4
	END            
