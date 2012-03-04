UniquE RAR File Library v0.4.0
The free file lib for the demoscene
multi-OS version (Win32, Linux and SunOS)
08.06.2002

1.) About the URARFileLib

     The UniquE RAR File Library gives you the ability to access RAR
     archives (any compression method supported in RAR v2.0 including
     Multimedia Compression and encryption) directly from your program
     with ease an by adding only 12kB (6kB UPX-compressed) additional
     code to your program.  Both 'solid' and 'normal' ('normal' is
     recommended for fast random access to the files!) archives are
     supported. Memory-to-memory and disk-to-memory decompression is
     supported. This FileLib is made for the demo scene, so it's
     designed for easy use within your demos and intros.  Please read
     the "license.txt" to learn more about thow you may use URARFileLib
     in your productions. And for sure, there is absolute no warranty!
     YOU USE IT AT YOUR OWN RISK. 

     Please check http://www.unrarlib.org for a newer version before you
     complain about bugs. Else, I would be very happy to read your
     feedback with suggestions, criticism, wishes etc. Please mail me a
     link to your demo if it's done, I'd like to have a lookt at it!

             Regards
             Christian Scheurer aka. UniquE of Vantage
             cs@unrarlib.org


2.) Win32

     If you use Microsoft's Visual C++ v6.0 and Windows 2000 you
     shouldn't have any problem since the Win32 system used to develop
     the URARFileLib.  Have a look at the samples and you'll understand
     how it works. Make sure to include the unrarlib.c and the
     unrarlib.h file in your project and your project include path. From
     this moment, you should be able to compile your project and access
     RAR archives with one line of code. The lib can be configured in
     with 'unrarlib.h'.



3.) Linux (i386)

     The very first versions of URARFileLib where created on a SuSE
     Linux system before I continued the development under Win32.
     Johannes Winkelmann was so kind to make the URARFileLib compile and
     work under Linux again. Please check the ./samples/unix - directory
     and have a look at the samples. It is as easy to use as the Win32
     version. And don't forget to configure the lib with 'unrarlib.h'.



4.) SunOS

     We had the chance to make a SunOS version on the workstations at
     school. It is nearly the same as in the Linux version, but you
     *have to* define 'NON_INTEL_BYTE_ORDER' because Motorola uses big
     endian encoding in its CPUs.  Configure this also in the
     'unrarlib.h'.



5.) configuration / advanced settings

     Remove the "_DO_CRC32_CHECK" and "_DEBUG_LOG" definitions in
     unrarlib.h if you want a tiny program with maximum speed speed and
     minimal size.  I suggest keeping the "_DO_CRC32_CHECK" definition
     in the release so you can detect and handle errors/data corruption
     and removing "_DEBUG_LOG" since I don't like software that writes
     logfiles somewhere without acknowledgement. And it may also cause
     errors if i.e. the disk is write protected. But "_DEBUG_LOG" is a
     great thing for searching errors.  Assembly extentions can be
     enabled by defining the symbol _USE_ASM.


6.) final words

     That's it. Thanks for reading this file. Please send me a link
     where I can download your production that uses the URARFileLib and
     give me a short feedback if you want.

	e-mail        : cs@unrarlib.org
        my home       : http://www.christianscheurer.ch
	vantage home  : http://www.vantage.ch
        unrarlib home : http://www.unrarlib.org

