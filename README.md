Some things to take care of:
* warning: ‘IOADDRESS’ is deprecated [-Wdeprecated-declarations]
* rdc_driver.c:690:5: warning: ‘Xalloc’ is deprecated (declared at /usr/include/xorg/os.h:224) [-Wdeprecated-declarations]
     if (!(pRDC->Options = xalloc(sizeof(RDCOptions))))

pScreen structure change: we need to pass either an index or the correct pointer.

rdc_driver.c: In function ‘RDCCloseScreen’:
rdc_driver.c:1601:42: warning: passing argument 1 of ‘pScreen->CloseScreen’ makes pointer from integer without a cast
     RetStatus = (*pScreen->CloseScreen) (scrnIndex, pScreen);
                                          ^
rdc_driver.c:1601:42: note: expected ‘ScreenPtr’ but argument is of type ‘int’


2D acceleration:
remove XAA part
enable EXA part
