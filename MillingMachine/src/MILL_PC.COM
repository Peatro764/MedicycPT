$!Fichier de commandes pour la conversion et le transfert des fichiers *.MIL 
$!de la version EYEPLAN PC sur la fraiseuse ` commandes numeriques
$ ecrit == "write sys$output"
$ def_dir:=$DISK4:[EYEAXP]
$ ret_dir:=$DISK4:[EYEAXP.EYEPC]
$ delete 'ret_dir'fichier_in.dat;*,'ret_dir'fichier_out.dat;*
$ port:=fraise 
$ ecrit "								"
$ ecrit "Demander la mise en mode 'READ-IN ALL PROGRAMS' de la fraiseuse a commandes numeriques"
$ ecrit "								"
$ loop:
$ set default 'ret_dir'
$ directory .MIL;
$ ecrit "								"
$ ecrit "	Copier coller le fichier a transferer ou FIN si termine "
$ ecrit "								"
$ inquire mill_file 
$ if mill_file .eqs. "FIN" then goto end
$!
$!Preparation du fichier fraiseuse
$!
$ open/write output_file fichier_in.dat
$ write output_file mill_file
$ close output_file
$ run 'ret_dir'trsfcol
$ open/read input_file fichier_out.dat
$ read input_file mill_out
$ close input_file
$!
$!Copie du fichier fraiseuse
$!
$ type/p 'ret_dir''mill_out'
$!!!! copy 'ret_dir''mill_out'  'port'
$
$ mess1="Le fichier "
$ mess2=" est transfere"
$ mess3=" renomme "
$ mess=mess1+mill_file+mess3+mill_out+mess2
$ ecrit "								"
$ ecrit "								"
$ ecrit mess
$ purge 'ret_dir'fichier_in.dat,'ret_dir'fichier_out.dat
$ goto loop
$ end:
$ set default 'def_dir'
$ exit
