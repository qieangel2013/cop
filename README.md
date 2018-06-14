### php扩展cop （兼容php7）
	    基于libco简单实现的协程
	    /usr/local/php/bin/phpize
	    ./configure --with-php-config=/usr/local/php/bin/php-config
	    make && make install
	    修改php.ini添加extension=cop.so
	    重启php-fpm
===================================
### 使用方法如下：
            首先安装php扩展cop.so
            phpize来安装
            然后在php文件调用
            dl('cop.so');或者phpini里加载
            function tetst($a){
				echo $a.'\n';
			}
			for ($i=0; $i <5000 ; $i++) { 
				cop_create('tetst',$i);
			}