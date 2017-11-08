#####################################################################
#																	 
#	Created by u'nSP IDE		05:48:04	01/01/11
#
#####################################################################




APPDIR	= C:\PROGRA~1\Sunplus\UNSPID~1.2D

OUTDIR	= .\Debug

CC	= $(APPDIR)\toolchain\gcc

AS	= $(APPDIR)\toolchain\xasm16

LD	= $(APPDIR)\toolchain\xlink16

AR	= $(APPDIR)\toolchain\xlib16

RESC	= $(APPDIR)\toolchain\resc

RM	= $(APPDIR)\toolchain\rm -f

INCLUDES	= -I"H:/Elevator/Elevator" -I"C:/Program Files/Sunplus/unSP IDE 2.6.2D/library/include" -I"C:/Program Files/Sunplus/unSP IDE 2.6.2D/library/include/sys"

BODY	= -body SPCE060A_061A -bfile "C:\Program Files\Sunplus\unSP IDE Common\Body\SPCE060A_061A.cpt"

BINFILE	= "$(OUTDIR)\Elevator.S37"

ARYFILE	= "$(OUTDIR)\Elevator.ary"

SBMFILE	= "$(OUTDIR)\Elevator.sbm"

OPT	= -S -gstabs -Wall -mglobal-var-iram

ASFLAGS	= -t2 -d

CASFLAGS	= -t2 -wpop

CFLAGS	= $(OPT) -B$(APPDIR)\toolchain\ $(INCLUDES) 

BINTYPE	= -as

LDFLAGS	= -listfile

EXTRAFLAGS	= 


OBJFILES	= \
	"$(OUTDIR)\Resource.obj" \
	"$(OUTDIR)\main.obj" \
	"$(OUTDIR)\Elevator.obj" 

"$(OUTDIR)\Resource.obj": "H:\Elevator\Elevator\Resource.asm" 
	$(AS) $(ASFLAGS) $(INCLUDES) -l "$(OUTDIR)\Resource.lst" -o "$(OUTDIR)\Resource.obj" "H:\Elevator\Elevator\Resource.asm" 

"$(OUTDIR)\main.asm": "H:\Elevator\Elevator\main.c" ".\Elevator.h" ".\SPCE061A.h" 
	set PATH="$(APPDIR)\toolchain\";%PATH% & \
	$(CC) $(CFLAGS) -o "$(OUTDIR)/main.asm" "H:/Elevator/Elevator/main.c" 

"$(OUTDIR)\main.obj": "$(OUTDIR)\main.asm"
	$(AS) $(CASFLAGS) $(INCLUDES) -l "$(OUTDIR)\main.lst" -o "$(OUTDIR)\main.obj" "$(OUTDIR)\main.asm" 

"$(OUTDIR)\Elevator.asm": "H:\Elevator\Elevator\Elevator.c" ".\Elevator.h" ".\SPCE061A.h" 
	set PATH="$(APPDIR)\toolchain\";%PATH% & \
	$(CC) $(CFLAGS) -o "$(OUTDIR)/Elevator.asm" "H:/Elevator/Elevator/Elevator.c" 

"$(OUTDIR)\Elevator.obj": "$(OUTDIR)\Elevator.asm"
	$(AS) $(CASFLAGS) $(INCLUDES) -l "$(OUTDIR)\Elevator.lst" -o "$(OUTDIR)\Elevator.obj" "$(OUTDIR)\Elevator.asm" 


.SUFFIXES : .c .asm .obj .s37 .tsk .res

all :	 BEFOREBUILD "$(OUTDIR)" $(BINFILE)

BEFOREBUILD :

"$(OUTDIR)" :
	if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

$(BINFILE) : $(OBJFILES) 
	$(LD) $(BINTYPE) $(ARYFILE) $(BINFILE) $(LDFLAGS) $(BODY) $(EXTRAFLAGS)

compile :	 $(OBJFILES)

clean :
	$(RM) "$(OUTDIR)\Resource.obj" 
	$(RM) "$(OUTDIR)\Resource.lst" 
	$(RM) "$(OUTDIR)\main.obj" 
	$(RM) "$(OUTDIR)\main.lst" 
	$(RM) "$(OUTDIR)\main.asm" 
	$(RM) "$(OUTDIR)\Elevator.obj" 
	$(RM) "$(OUTDIR)\Elevator.lst" 
	$(RM) "$(OUTDIR)\Elevator.asm" 
	$(RM) $(BINFILE) $(SBMFILE) 

.c.asm:
	$(CC) $(CFLAGS) $(INCLUDES) -o "$(OUTDIR)/$@" $<

.asm.obj:
	$(AS) $(ASFLAGS) $(INCLUDES) -o "$(OUTDIR)/$@" $<

