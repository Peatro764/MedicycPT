$ ! MILL_TO_VAX.COM
$ ! Ce programme permet de lire des fichiers envoyes par la fraiseuse numerique
$ ! les fichiers venant de la fraiseuse sont formates (les line feed sont effaces)
$ ! ensuite les programmes sont ranges dans le directory [CHARLY.MILL.PROGRAMME]
$ ! Requires one parameter - the output port
$ !
$ ! Le 22/11/94 rajout de ON CONTROL_Y... qui permet au programme de sortir 
$ ! proprement (par un EXIT) apres un control_y.
$ !
$ ! Le 1/08/96 rajout de format_program qui permet de formater les programme 
$ ! sans sortir du programme.
$ ! Suppression du CRTL_Z et rajout d'un TYPE/CONFIRM apres le CRTL_Y.
$ !
$ !
$ ON CONTROL_Y THEN GOTO FINISH
$ IF P1 .NES. "" THEN GOTO OK
$ WRITE SYS$OUTPUT " "
$ INQUIRE P1 "Which input port to use (return = FRAISE) ?"
$ IF P1 .EQ. "" THEN GOTO FRAISE
$OK:
$ P1 = F$EDIT(P1,"UPCASE")
$ IF F$LOCATE(":",P1) .EQ. F$LEN(P1) THEN P1 = P1 + ":"
$ GOTO SUITE
$ !
$FRAISE:
$ P1 = "FRAISE"
$SUITE:
$ IF .NOT. $STATUS THEN EXIT
$ WRITE SYS$OUTPUT " "
$ WRITE SYS$OUTPUT "Fichier envoyé part le port ''P1'"
$ WRITE SYS$OUTPUT " "
$ WRITE SYS$OUTPUT "Utilisez la fraiseuse pour 'ENVOYER LES PROGRAMMES'"
$ !
$ ! Fetch file names and output them
$LOOP:
$ WRITE SYS$OUTPUT " "
$ WRITE SYS$OUTPUT "Tape le chiffre qui correspond au DIRECTORY"
$ WRITE SYS$OUTPUT " "
$ WRITE SYS$OUTPUT "  Modulateur   = 1"
$ WRITE SYS$OUTPUT "  Collimateur  = 2"
$ WRITE SYS$OUTPUT "  Programmes   = 3"
$ WRITE SYS$OUTPUT "  Electricité  = 4"
$ WRITE SYS$OUTPUT "  Divers       = 5"
$ WRITE SYS$OUTPUT " "
$ INQUIRE MILL_DIR "       Choix [Return = END]"
$ WRITE SYS$OUTPUT " "
$ WRITE SYS$OUTPUT " "
$ IF MILL_DIR .EQ. "" THEN GOTO EXIT
$ WRITE SYS$OUTPUT " "
$ IF MILL_DIR .EQ. 1 THEN  DEF_DIR = "[CHARLY.MILL.MODU]"
$ IF MILL_DIR .EQ. 2 THEN  DEF_DIR = "[CHARLY.MILL.COLLI]"
$ IF MILL_DIR .EQ. 3 THEN  DEF_DIR = "[CHARLY.MILL.PROGRAM]"
$ IF MILL_DIR .EQ. 4 THEN  DEF_DIR = "[CHARLY.MILL.ELEC]"
$ IF MILL_DIR .EQ. 5 THEN  DEF_DIR = "[CHARLY.MILL.DIVERS]"
$ INQUIRE MILL_FILE "Nom du fichier fraiseuse,<BACK_MILL_numero malade>"
$ WRITE SYS$OUTPUT " Fichier [5;1men attente[22;25m de transfert."
$ WRITE SYS$OUTPUT " "
$ WRITE SYS$OUTPUT " Faire [5;1mCtrl_Y[22;25m quand le transfert est fini"
$ IF MILL_FILE .EQS. "" THEN GOTO FINISH
$ MILL_FILE = F$EDIT(MILL_FILE,"UPCASE")
$ ! Default file extension to .MIL
$ FILE_NAME = MILL_FILE
$ LEN = F$LEN(FILE_NAME)
$ END_DIR = F$LOCATE("]",FILE_NAME)
$ IF END_DIR .NE. LEN THEN FILE_NAME = F$EXTRACT(END_DIR,LEN-END_DIR,FILE_NAME)
$ IF F$LOCATE(".",FILE_NAME) .EQ. F$LEN(FILE_NAME) THEN -
    MILL_FILE = MILL_FILE + ".MIL"
$ ! directory 
$ IF F$LOCATE("[",MILL_FILE) .EQ. F$LEN(MILL_FILE) THEN -
    MILL_FILE = DEF_DIR + MILL_FILE
$ ! Default device to $DISK5:
$ IF F$LOCATE(":",MILL_FILE) .EQ. F$LEN(MILL_FILE) THEN -
    MILL_FILE = "$DISK6:" + MILL_FILE
$ COPY 'P1' 'MILL_FILE' 
$ FINISH:
$ TYPE /CONF/NOPAGE 'MILL_FILE'
$ WRITE SYS$OUTPUT " "
$ INQUIRE CHOIX "  ==>  Formater le programme [Return=Yes] ?"
$ WRITE SYS$OUTPUT " "
$ IF CHOIX .NES. "" THEN GOTO TEST
$ SET DEF 'DEF_DIR'
$ @[CHARLY.MILL]format_program
$ TEST:
$ WRITE SYS$OUTPUT " "
$ INQUIRE CHOIX "  ==>  Envoi d'un autre programme [Return=Yes] ?"
$ WRITE SYS$OUTPUT " "
$ IF CHOIX .EQS. "" THEN GOTO LOOP
$ EXIT:
$ EXIT
