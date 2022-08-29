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
  
.. code-block:: sh
  
  git archive --remote=https://github.com/xsbee/libido | tar -t
  make -C libido
  
And if you want to build examples,

.. code-block:: sh
  
  make -C libido/examples
  
FAQ
---

Why you made this?
~~~~~~~~~~~~~~~~~~

One day I and Sig were hanging about his house. Sig told his parents aren't home. So we put in the DVD drive. After doing stuff (you know what that is),
he suddenly proposed me an idea of fetching catalog of Hentai from hanime.tv and back then .NET was very expensive in Austria and we also hated JavaScript.
So I and Sig coauthored this program. Sig died, so all the code now belongs to me.

Isn't it bad?
~~~~~~~~~~~~~

No. Many `famous people <https://en.wikipedia.org/wiki/Albert_Einstein#Early_life_and_education>`_ succumb to it.

How to play around?
~~~~~~~~~~~~~~~~~~~

If you have built examples, then a ``libido-query`` tool have been generated, invoke ``-h`` method on it to look what you can do with it.
For example, you can query a list of uncensored titles like this.

.. code-block:: sh

  $ ./libido-query -o likes -t uncensored
  ---
  Itadaki! Seieki (1226)

  <p>Based on the erotic manga by Doumou.</p>

  <p>Succubus Setogaya Mari is a part-vampire who takes her nourishment not from blood but from drinking semen. As she performs oral sex for energy, various things can happen; for example, her breasts might simply grow during the act.</p>


    duration 0:22:58
    by Pashmina (65)
    viewed by 6400859 otakus
    38113 likes 266 dislikes
    uncensored? oh yeah!
    has
      big boobs blow job bondage boob job comedy cosplay facial gangbang ntr public sex rape school girl uncensored monster anal hd nekomimi
    aka
      I`ll Have Your Semen
      Itadaki! Seieki
      Vampire Vixen
      いただきっ! セーエキ
      잘먹을게 정액
    cover https://git-covers.pages.dev/images/itadaki-seieki-IHqaP.jpg
    poster https://git-posters.pages.dev/images/itadaki-seieki.jpg
    ranking 95
    released at Thu Mar 27 20:30:00 2014
    uploaded at Tue Jun  7 05:32:39 2016
    downloaded 179867 times
    url https://hanime.tv/videos/hentai/itadaki-seieki
  ---
  ...

You can install it via ``su -c "install libido/examples/libido-query /usr/local/bin"``. 

.. [1] https://github.com/json-c/json-c
.. [2] https://curl.se/libcurl/ 
