Instruments for preparing independent kungfu environment 
==========

* yum install rpm-build (sudo apt install rpm)

* yum install numactl (sudo apt install numactl)

* Compiling Kungfu

    * install log4cplus [https://github.com/log4cplus/log4cplus]
        * take a look at issue: https://github.com/log4cplus/log4cplus/issues/169
        
    * install pid-2.1.1
        * install dependency nose-1.3.7 first (yum install python-nose) (sudo apt install python-nose)
        * install pip (sudo /usr/bin/pip install pid)

    * install rfoo [https://github.com/aaiyer/rfoo]
        * install dependency Cython first (sudo apt install cython)
        * install rfoo by source (sudo /usr/bin/python setup.py build_ext --inplace; sudo /usr/bin/python setup.py install)

    * install supervisor
        * install dependency meld3 (yum install python-meld3) (sudo apt install python-meld3)
        * install supervisor (sudo apt install supervisor)

    * install boost-1.62 (or other versions)
        * install dependency libpython-dev (sudo apt install libpython-dev)
        * make sure folder /opt/kungfu/toolchain/boost-1.62.0 exists
        * ./bootstrap.sh --prefix=/opt/kungfu/toolchain/boost-1.62.0
        * ./b2 install --prefix=/opt/kungfu/toolchain/boost-1.62.0

    * install cmake-[3.8.*]

    * library search path:
        ```
        # # add LD_LIBRARY_PATH for all users
        # The cleanest way to add a library to the search path is by adding a file to /etc/ld.so.conf.d/
        
        echo "/usr/local/lib" >> /etc/ld.so.conf.d/log4cplus-2.0.so.conf  # suppose log4cplus is installed in /usr/local/lib
        
        # load the new configuration, run the following command as a root
        
        ldconfig -v
        ```
        
    * compiling kungfu and install kungfu
        * cp libpython2.7.so as libpython.so
        * rename boost-1.62.0 in kungfu/build/cmake_install.cmake as boost-x.xx.x if anther version of boost is used
        * rename `wheel` in kungfu/rpm/scripts/post_install.sh as `sudo` if system is ubuntu
    
    * modify kungfu/rpm/etc/sysconfig/kungfu (/opt/kungfu/master/etc/sysconfig/kungfu)
        * add /opt/kungfu/master/lib/python2.7/site-packages into PYTHONPATH
    * modify kungfu/rpm/bin/yjj (/opt/kungfu/master/bin/yjj)
        * add /opt/kungfu/master/lib/python2.7/site-packages into PYTHONPATH
    
    * if you want to run yjj command with sudo
        * revise env_reset to env_keep+="PYTHONPATH"

**Contributed by Aimin Huang.**
**Supplemented by Fangxin Hu.**
