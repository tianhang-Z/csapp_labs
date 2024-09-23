## 概述

* 对应第二章 信息的表示和处理

* 这一章主要是整数、浮点数的表示和运算 

* 重点是理解补码的表示，补码的加法（理解正负溢出），利用取反~计算相反数，用补码做减法运算  $ 5-3=5+(-3)$ 

* 算数移位和逻辑移位的区别（无符号数为逻辑移位，有符号数是算术移位，右移时左端填充符号位）
*  对整型做逻辑运算！        !0x41=0x00

* 整型的除法是舍弃结果的小数位，正数向下取整 ，负数向上取整 
* 利用移位及加减运算实现乘除法

* 不使用减法 实现整数的大小比较

* 浮点数的表示

## 答案代码

修改tests.c

```c
/* 
 * bitAnd - x&y using only ~ and | 
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
//德摩根公式 ~x&y=(~x)|(~y)  ~(x|y)=(~x)&(~y)
//~的优先级很高 要加括号
int bitAnd(int x, int y) {
  return ~((~x)|(~y));
}
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
// 移位后, 与mask：0xff位与
int getByte(int x, int n) {
  return (x>>(n<<3))&0xff;
}
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3 
 */
//对于有符号数 默认移位是算数移位 由于整型int的负数最高位是1 右移时会对最高位补1
//对于无符号数 移位则是逻辑移位
//补码的好处：以-4为例 其二进制数为11111111 11111111 11111111 11111100  还差4变成0x0000 0000
//其算数右位1位后  还差2变成 0x0000 0000 即从-4变成了-2
//采用补码可以使负数的二进制加减法更方便
int logicalShift(int x, int n) {
  //实验要求使用0-255以内的常数
  // unsigned int mask=0xffffffff;  
  // mask>>=n;

  //目的是构造一个以0开头的mask  
  // 逻辑反 ~2=-3  ~1=-2  ~0=0xffff ffff=-1
  int mask=((0x1<<(32+~n))+~0)  |(0x1<<(32+~n));
  return (x>>n)&mask;
}
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
//很巧妙  归并 相邻奇偶位->相邻四位->相邻8位->相邻字节->...
int bitCount(int x) {
  // int mask1=0x55555555;
  // int mask2=0x33333333;
  // int mask3=0x0f0f0f0f;
  // int mask4=0x00ff00ff;
  // int mask5=0x0000ffff;
  int _mask1=(0x55<<8)|0x55;
  int mask1=_mask1|(_mask1<<16);
  int _mask2=(0x33<<8)|0x33;
  int mask2=_mask2|(_mask2<<16);
  int _mask3=(0x0f<<8)|0x0f;
  int mask3=_mask3|(_mask3<<16);
  int mask4=0xff|(0xff<<16);
  int mask5=0xff|(0xff<<8);
  int ans=(x&mask1)+((x>>1)&mask1);
  ans=(ans&mask2)+((ans>>2)&mask2);
  ans=(ans&mask3)+((ans>>4)&mask3);
  ans=(ans&mask4)+((ans>>8)&mask4);
  ans=(ans&mask5)+((ans>>16)&mask5);
  return ans;
}
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */ 
//x为0  !x返回1
//逐层或
//前16位和后16位进行位或
//后16位的前8位和后8位进行或
//...如此循环 最后进行最后两位的或 看能否得到1
int bang(int x) {
  x=(x>>16)|x;
  x=(x>>8)|x;
  x=(x>>4)|x;
  x=(x>>2)|x;
  x=(x>>1)|x;
  return ~x&0x1;
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
// two's complement 补码
//补码最小值  
int tmin(void) {
  return 0x1<<31;
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
// ~0=-1 ~1=-2  ~2=-3
// 以x=5为例 至少要0101四位  
// 假如n=3,把第3位左移29次到int的符号位，再右移29次，得到的新数和原数不同了
// 加入n>=4，把最高的第n位移动到int符号位，再移动到原位，新数和原数相同，n足够
// 若x为负数，同理，只要第一个0不移动到int的符号位即可
// 用异或判断两数是否相同
// 注：这个判题有bug 0x80000000 可以用32bit表示  //n=32必然可以
int fitsBits(int x, int n) {
  int shift_bit=33+~n;
  int new_num=(x<<shift_bit)>>shift_bit;
  int ans=!(new_num^x);        
  return ans;
}
/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
//  
int divpwr2(int x, int n) {
    int bias=(x>>31)&((1<<n)+~0);
    return (x+bias)>>n;
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
//利用~实现减法
int negate(int x) {
  return ~x+1;
}
/* 
 * isPositive - return 1 if x > 0, return 0 otherwise 
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
//需满足不为0 : x当x不为0时返回1  取两次非 使结果为只有一位
//还需满足符号位不为1
int isPositive(int x) {
  return (!!x)&!((x>>31)&0x1);
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
//需x-y<=0 
//而-y=~y+1
//若x-y<=0 则x+~y+1<=0  x+~y<0 
//特殊的 x=0x80000000 y=0x7fffffff  x+~y=0;  负溢出了
// x=0x7ffffffff y=0x80000000   x+~y=-1  正溢出了
int isLessOrEqual(int x, int y) {
  int val_op=((x+~y)>>31)&0x1;       
  int x_op=(x>>31)&1;
  int y_op=(y>>31)&1;
  //x_op=0 y_op=1 return 0    
  //  !(!x_op&y_op) = (x_op|!y_op) 
  //x_op=1 y_op=0 return 1
  //val_op=1 return 1
  return !(!x_op&y_op)&(val_op|(x_op&!y_op));
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
//找到1的最高位
//答案思路：二分
int ilog2(int x) {
  int ans=1; 
  //!!x当x不为0返回1
  ans=(!!(x>>16))<<4;   //看是在前16位还是后16位  对应ans记为16或0
  ans=ans+((!!(x>>(ans+8)))<<3);   //在上次的基础上 看是在前8位还是后8位 ans加上8或0
  ans=ans+((!!(x>>(ans+4)))<<2);
  ans=ans+((!!(x>>(ans+2)))<<1);
  ans=ans+((!!(x>>(ans+1)))<<0);
  return ans;
}
/* 
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
//需识别出NaN 
unsigned float_neg(unsigned uf) {
  if (((uf<<1)^0xffffffff)<0x00ffffff) {
    return uf;
  }
  else return uf^0x80000000;       //最高位与1异或 取反
}
/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
//float s exp(8bits) frac(23bits，0-22)
//E=e-bias  bias=127
//frac不带开头隐含的1.
unsigned float_i2f(int x) {
  int e=0;
  int n=0xffffffff;
  int tmp=0;
  int tmp2=0;
  int cp=0;
  int cp2=0;   //x的符号位
  int sign=x&0x80000000;
  if(x==0x80000000) {
    return 0xcf000000;     //-2^31;  (31+127)=158=0b1001 1110  这是exp   
  }
  if(x==0){
    return 0;
  }
  if(sign){
    x=-x;  //若x为负,将其变为正值
  } 

  x=x&0x7fffffff;  //去掉x的符号位
  tmp=x;
  //获取x的1的最高位  （0-31）
  while(tmp>0){
    tmp=tmp>>1;
    n++;
  }
  x=x-(0x1<<n); //去掉x的最高位1
  //如果最高位小于等于23位 则令frac等于去除1最高位后的x 又frac左侧默认带个1,0
  //frac左移23-n次后，x 阶码E为n，隐含的1和原来x的最高位对齐 
  if(n<24){
    x=x<<(23-n);
  }//如果n大于23，则需x最高位和隐含的1对齐
  else{
    tmp2=x>>(n-23);
    cp2=1<<(n-24);      //eg:n=26,cp2=0b100,(cp2<<1)-1=0b111
    cp=x&((cp2<<1)-1);  //获取x右移后变成tmp2后消失的那部分,即舍去的部分
    if(cp<cp2){    //舍去部分的最高位不为1，则直接舍去
      x=tmp2;
    }else{  //舍去部分的最高位为1，则向上舍入，tmp2进1
      if(tmp2==0x7fffff){
        x=0;
        n++;
      }
      else{      
        if(cp==cp2){    //中间值向偶数舍入
          x=(tmp2&0x1)+tmp2;
        }
        else x=tmp2+1;
      }
    }
  }
  e=(n+127)<<23;  //n为E E=e-bias
  return sign|e|x;
}
/* 
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
  int tmp=uf;
  int sign=uf&0x80000000;
  int exp=uf&0x7f800000;
  int f=uf&0x7fffff;
  tmp=tmp&0x7fffffff;  //移除sign
  if(exp==0x0){  //处理非规格化数{
    tmp=(tmp<<1)|sign;
    return tmp;
  } 
  else if((exp>>23)==0xff){     //处理无穷和NaN
    return uf;
  }
  else{
    if((exp>>23)+1==0xff)  return sign|0x7f800000;
    else{
      return sign|(((exp>>23)+1)<<23)|f;
    }
  }
  return tmp;
}

```





