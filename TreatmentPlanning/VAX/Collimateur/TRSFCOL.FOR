c------------------------------------------------------------------------------
c	Lecture des donnes collimateur eyeplan et transcription en coordonnes
c	machine decembre 03
c------------------------------------------------------------------------------

	dimension donnees (100),x_donnees(100),y_donnees(100),ent22(6)
	dimension ix(100),iy(100),ligne_c(20),i_ligne_c(20)
        character*50 donnees,ent1,ent2,ent3,test*1,numero,nom,oeil
	character*50 fichier_initial,numeronom*13
	integer ent22
	real x_donnees,y_donnees

c	Lecture du fichier de coordonnees initiales
                 
	open (unit=20,file='fichier_in.dat',status='old')
	read (20,50) fichier_initial
	close (unit=20)
	open (unit=30,file=fichier_initial,status='old')
	do i=1,100
		read (30,20,end=10) donnees (i)
		i_donnees=i
	end do
 10	continue                                            
	i_x=i_donnees-3
	rewind 30

        read(30,20) ent1
	read(30,20) ent2
	read(30,20) ent3
	do i=1,100
		ix(i)=6
		iy(i)=6
	end do
	do i=1,i_x
		read (30,*) x_donnees (i),y_donnees(i)
		if (abs(x_donnees (i)).ge.10.0) ix(i)=7 
		if (abs(y_donnees (i)).ge.10.0) iy(i)=7 
	end do
	rewind 30

	j=1
	do i=1,49
	        test=ent2(i:(i+1))
		if (test.eq.'"') then 
			ent22(j)=i
			j=j+1
		end if
	end do

	numero=ent2((ent22(1)+1):(ent22(2)-1))
	i_numero=-ent22(1)+ent22(2)-1
	nom=ent2((ent22(3)+1):(ent22(4)-1))
	oeil=ent2((ent22(5)+1):(ent22(6)-1))

 20	format (A50)
 40	format (A1)
 50	format (A50)
 70	format (A13)
                                 
                 
c	introduction

	numeronom=numero(1:5)//nom(1:4)//'.mil'
	open (unit=30,file='fichier_out.dat',status='new')
	write (30,70) numeronom
	close (unit=30)

	open (unit=40,file=numeronom,status='new')

	write (40,1000) numero
 1000   format(/X,'0 BEGIN PGM ',A<i_numero>,' MM'/
	1       X,'1 BLK FORM 0.1 Z X-17 Y-17 Z-8'/
	2       X,'2 BLK FORM 0.2 X+17 Y+17 Z+0'/
	3       X,'3 TOOL DEF 1 L+0 R+1,60'/
	4       X,'4 TOOL DEF 2 L+0 R+1,50'/
	5       X,'5 TOOL DEF 3 L+0 R+1,50'/
	6       X,'6 TOOL CALL 1 Z S 3000'/
	7       X,'7 L X+0 Y+0 R0 F MAX M03'/
	8       X,'8 L Z+2 R0 F MAX M'/
	9       X,'9 L Z+0 R0 F100 M'/
	1       X,'10 LBL 1'/
	2       X,'11 L IZ-1,05 R0 F100 M')
                                 
c	developpement

        write(40,1100) -x_donnees(1),y_donnees(1)
 1100   format(X,'12 L X',SP,F<ix(1)>.3,' Y',F<iy(1)>.3,' RL F M')
        write(40,1200) -x_donnees(2),y_donnees(2)
 1200   format(X,'13 L X',SP,F<ix(2)>.3,' Y',F<iy(2)>.3,' R F M')
	ligne= 14
	i_ligne=2
        do i=3,i_x
		write (40,1300) ligne,-x_donnees(i),y_donnees(i)
		if (ligne.ge.100) i_ligne=3   
		ligne=ligne+1
	end do
 1300	format(X,I<i_ligne>,' L X',SP,F<ix(i)>.3,' Y',F<iy(i)>.3,' R F M')

c	conclusion                       
	
	i_concl=17
	do i=1,i_concl
		i_ligne_c(i)=2
		ligne_c(i)=ligne-1+i
		if (ligne_c(i).ge.100) i_ligne_c(i)=3
	end do	
        write(40,1400) (ligne_c(i), i=1,i_concl)
 1400   format(X,I<i_ligne_c(i)>,' L X+0 Y+0 R0 F M'/
	1       X,I<i_ligne_c(i)>,' LBL 0'/
	2       X,I<i_ligne_c(i)>,' CALL LBL 1 REP 7 /7'/
	3       X,I<i_ligne_c(i)>,' TOOL CALL 2 Z S 3000'/
	4       X,I<i_ligne_c(i)>,' L Z-9 R0 F MAX M03'/
	5       X,I<i_ligne_c(i)>,' CALL LBL 1 REP'/
	6       X,I<i_ligne_c(i)>,' TOOL CALL 3 Z S 3000'/
	7       X,I<i_ligne_c(i)>,' L Z-9 R0 F MAX M03'/
	8       X,I<i_ligne_c(i)>,' CALL LBL 1 REP'/
	6       X,I<i_ligne_c(i)>,' L Z+10 R0 F MAX M'/
	7       X,I<i_ligne_c(i)>,' CC X+0 Y+0'/
	8       X,I<i_ligne_c(i)>,' LP PR+22,5 PA+45 R0 F MAX M03'/
	9       X,I<i_ligne_c(i)>,' L Z+2 R0 F MAX M'/
	1       X,I<i_ligne_c(i)>,' L Z-2 R0 F40 M'/
	2       X,I<i_ligne_c(i)>,' LP PR+18,5 PA+45 R0 F M'/
	3       X,I<i_ligne_c(i)>,' L Z+2 R0 F100 M'/
	4       X,I<i_ligne_c(i)>,' L R F M25')

	i_ligne_cc=2
	ligne_cc=ligne_c(i_concl)+1
	if (ligne_cc.ge.100) i_ligne_cc=3  
	write (40,1500) ligne_cc,numero
 1500   format(X,I<i_ligne_cc>,' END PGM ',A<i_numero>,' MM')
	close (unit=40)
	end
