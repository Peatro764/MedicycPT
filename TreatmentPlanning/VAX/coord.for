EYEAXP>type CORDE.FOR
        dimension arc(10),corde(10)
        write (*,*) 'Entrer le diametre du globe oculaire en mm'
        read (*,*) dia
        write (*,*) 'Entrer les mesures d''arc en mm, taper 0 pour cloturer'
        write (*,*) 'Faire return a chaque entree'
        do i=1,10
                read (*,*) arc(i)
                corde(i)=dia*sin(arc(i)/dia)
                if (corde(i).le.'0.01') then
                        imax=i
                        goto 1
                end if
        end do
    1   continue        
        write (*,5) 
    5   format(T3,'Arc',T12,'Corde')
        do i=1,imax-1
                write (*,10) arc(i),corde(i)
        end do
   10   format(F6.1,T10,F6.1)
        end
