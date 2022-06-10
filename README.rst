======
libido
======

This is a library to interact with **hanime.tv** API, presenting a familiar C interface.

Goals
-----
I. Catering libido requirements.
II. Bare memory consumption.
III. Easy FFI via most languages.

Building
--------

Dependencies
~~~~~~~~~~~~

- libcurl [1]_
- libjson-c [2]_

Instructions
~~~~~~~~~~~~
  
.. code:: bash
  git archive --remote=https://github.com/xsbee/libido | tar -t 
  cd libido
  make
  
FAQ
---

Isn't it bad?
~~~~~~~~~~~~~

No. Many `famous people <https://en.wikipedia.org/wiki/Albert_Einstein#Early_life_and_education>`_ succumb to it.

Why in C?
~~~~~~~~~

`See this <https://www.youtube.com/watch?v=1S1fISh-pag>`_.

.. [1] https://github.com/json-c/json-c
.. [2] https://curl.se/libcurl/ 
