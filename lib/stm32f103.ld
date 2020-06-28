/* Highest address of the user mode stack */
_estack = 0x20005000;    /* end of RAM */

/* Memories definition */
MEMORY
{
  RAM (xrw)     : ORIGIN = 0x20000000, LENGTH = 20K
  ROM (rx)      : ORIGIN = 0x8000000, LENGTH = 64K
}

/* Sections */
SECTIONS
{
  /* The program code and other data into ROM memory */
  /* The program code and other data into ROM memory */
  .text : ALIGN(4)              /* таблица прерываний и затем код программы в начало флэша */
  {
    FILL(0xff)
    /*. = ALIGN(4); */     /* выравнивание по 4 байтам t.е., если перед этой секцией курсор указывает на адрес 0x00000101, то .text начнется со 0x00000104*/
    *(.vectors)
    KEEP(*(.vectors))
    *(.text)           /* .text sections (code) */
  } >ROM                /* все экспортируемые функции и их адреса (которые указывают куда-то в секцию .text)*/
  

  /* Constant data into ROM memory*/
  .rodata :ALIGN(4)
  {
    /*. = ALIGN(4);*/
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
  } >ROM

  /* Initialized data sections into RAM memory */
  .data :ALIGN(4)
  {
    _data = .;   /* в _data сохраняется текущее положение курсора*/
    *(SORT_BY_ALIGNMENT(.data*)) /* сортировка по выравниванию */
    . = ALIGN(4);       
    /* переменные (глобальные и статические), которые инициализированы - затем передаются в RAM */
    *(.data)           /* .data sections */
    _edata = .;   /* в _edata сохраняется текущее положение курсора*/
    
  } >RAM AT> ROM=0xff       /*задает шаблон «заливки». Если в выходной секции образуются неадресованные байты, их значение будет установлено в значение байта-заполнителя. */


  /* Uninitialized data section into RAM memory */
  
  .bss : ALIGN(4)
  {
    _bss = .;
    *(.bss)             /* не инициализированные Глобальные переменные*/
    *(COMMON)           /* эта секция живет до линковки и затем переписывается в .bss (неинициализированные переменные) или в .data*/
    /*в С++ This allows you to have the same variable defined in different compilation units. */

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM

}
