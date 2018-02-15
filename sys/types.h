1	/* Copyright (C) 1991-2016 Free Software Foundation, Inc.
2	   This file is part of the GNU C Library.
3	
4	   The GNU C Library is free software; you can redistribute it and/or
5	   modify it under the terms of the GNU Lesser General Public
6	   License as published by the Free Software Foundation; either
7	   version 2.1 of the License, or (at your option) any later version.
8	
9	   The GNU C Library is distributed in the hope that it will be useful,
10	   but WITHOUT ANY WARRANTY; without even the implied warranty of
11	   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
12	   Lesser General Public License for more details.
13	
14	   You should have received a copy of the GNU Lesser General Public
15	   License along with the GNU C Library; if not, see
16	   <http://www.gnu.org/licenses/>.  */
17	
18	/*
19	 *        POSIX Standard: 2.6 Primitive System Data Types        <sys/types.h>
20	 */
21	
22	#ifndef        _SYS_TYPES_H
23	#define        _SYS_TYPES_H        1
24	
25	#include <features.h>
26	
27	__BEGIN_DECLS
28	
29	#include <bits/types.h>
30	
31	#ifdef        __USE_MISC
32	# ifndef __u_char_defined
33	typedef __u_char u_char;
34	typedef __u_short u_short;
35	typedef __u_int u_int;
36	typedef __u_long u_long;
37	typedef __quad_t quad_t;
38	typedef __u_quad_t u_quad_t;
39	typedef __fsid_t fsid_t;
40	#  define __u_char_defined
41	# endif
42	#endif
43	
44	typedef __loff_t loff_t;
45	
46	#ifndef __ino_t_defined
47	# ifndef __USE_FILE_OFFSET64
48	typedef __ino_t ino_t;
49	# else
50	typedef __ino64_t ino_t;
51	# endif
52	# define __ino_t_defined
53	#endif
54	#if defined __USE_LARGEFILE64 && !defined __ino64_t_defined
55	typedef __ino64_t ino64_t;
56	# define __ino64_t_defined
57	#endif
58	
59	#ifndef __dev_t_defined
60	typedef __dev_t dev_t;
61	# define __dev_t_defined
62	#endif
63	
64	#ifndef __gid_t_defined
65	typedef __gid_t gid_t;
66	# define __gid_t_defined
67	#endif
68	
69	#ifndef __mode_t_defined
70	typedef __mode_t mode_t;
71	# define __mode_t_defined
72	#endif
73	
74	#ifndef __nlink_t_defined
75	typedef __nlink_t nlink_t;
76	# define __nlink_t_defined
77	#endif
78	
79	#ifndef __uid_t_defined
80	typedef __uid_t uid_t;
81	# define __uid_t_defined
82	#endif
83	
84	#ifndef __off_t_defined
85	# ifndef __USE_FILE_OFFSET64
86	typedef __off_t off_t;
87	# else
88	typedef __off64_t off_t;
89	# endif
90	# define __off_t_defined
91	#endif
92	#if defined __USE_LARGEFILE64 && !defined __off64_t_defined
93	typedef __off64_t off64_t;
94	# define __off64_t_defined
95	#endif
96	
97	#ifndef __pid_t_defined
98	typedef __pid_t pid_t;
99	# define __pid_t_defined
100	#endif
101	
102	#if (defined __USE_XOPEN || defined __USE_XOPEN2K8) \
103	    && !defined __id_t_defined
104	typedef __id_t id_t;
105	# define __id_t_defined
106	#endif
107	
108	#ifndef __ssize_t_defined
109	typedef __ssize_t ssize_t;
110	# define __ssize_t_defined
111	#endif
112	
113	#ifdef        __USE_MISC
114	# ifndef __daddr_t_defined
115	typedef __daddr_t daddr_t;
116	typedef __caddr_t caddr_t;
117	#  define __daddr_t_defined
118	# endif
119	#endif
120	
121	#if (defined __USE_MISC || defined __USE_XOPEN) && !defined __key_t_defined
122	typedef __key_t key_t;
123	# define __key_t_defined
124	#endif
125	
126	#if defined __USE_XOPEN || defined __USE_XOPEN2K8
127	# define __need_clock_t
128	#endif
129	#define        __need_time_t
130	#define __need_timer_t
131	#define __need_clockid_t
132	#include <time.h>
133	
134	#ifdef __USE_XOPEN
135	# ifndef __useconds_t_defined
136	typedef __useconds_t useconds_t;
137	#  define __useconds_t_defined
138	# endif
139	# ifndef __suseconds_t_defined
140	typedef __suseconds_t suseconds_t;
141	#  define __suseconds_t_defined
142	# endif
143	#endif
144	
145	#define        __need_size_t
146	#include <stddef.h>
147	
148	#ifdef __USE_MISC
149	/* Old compatibility names for C types.  */
150	typedef unsigned long int ulong;
151	typedef unsigned short int ushort;
152	typedef unsigned int uint;
153	#endif
154	
155	/* These size-specific names are used by some of the inet code.  */
156	
157	#if !__GNUC_PREREQ (2, 7)
158	
159	/* These types are defined by the ISO C99 header <inttypes.h>. */
160	# ifndef __int8_t_defined
161	#  define __int8_t_defined
162	typedef        char int8_t;
163	typedef        short int int16_t;
164	typedef        int int32_t;
165	#  if __WORDSIZE == 64
166	typedef long int int64_t;
167	#  else
168	__extension__ typedef long long int int64_t;
169	#  endif
170	# endif
171	
172	/* But these were defined by ISO C without the first `_'.  */
173	typedef        unsigned char u_int8_t;
174	typedef        unsigned short int u_int16_t;
175	typedef        unsigned int u_int32_t;
176	# if __WORDSIZE == 64
177	typedef unsigned long int u_int64_t;
178	# else
179	__extension__ typedef unsigned long long int u_int64_t;
180	# endif
181	
182	typedef int register_t;
183	
184	#else
185	
186	/* For GCC 2.7 and later, we can use specific type-size attributes.  */
187	# define __intN_t(N, MODE) \
188	  typedef int int##N##_t __attribute__ ((__mode__ (MODE)))
189	# define __u_intN_t(N, MODE) \
190	  typedef unsigned int u_int##N##_t __attribute__ ((__mode__ (MODE)))
191	
192	# ifndef __int8_t_defined
193	#  define __int8_t_defined
194	__intN_t (8, __QI__);
195	__intN_t (16, __HI__);
196	__intN_t (32, __SI__);
197	__intN_t (64, __DI__);
198	# endif
199	
200	__u_intN_t (8, __QI__);
201	__u_intN_t (16, __HI__);
202	__u_intN_t (32, __SI__);
203	__u_intN_t (64, __DI__);
204	
205	typedef int register_t __attribute__ ((__mode__ (__word__)));
206	
207	
208	/* Some code from BIND tests this macro to see if the types above are
209	   defined.  */
210	#endif
211	#define __BIT_TYPES_DEFINED__        1
212	
213	
214	#ifdef        __USE_MISC
215	/* In BSD <sys/types.h> is expected to define BYTE_ORDER.  */
216	# include <endian.h>
217	
218	/* It also defines `fd_set' and the FD_* macros for `select'.  */
219	# include <sys/select.h>
220	
221	/* BSD defines these symbols, so we follow.  */
222	# include <sys/sysmacros.h>
223	#endif /* Use misc.  */
224	
225	
226	#if (defined __USE_UNIX98 || defined __USE_XOPEN2K8) \
227	    && !defined __blksize_t_defined
228	typedef __blksize_t blksize_t;
229	# define __blksize_t_defined
230	#endif
231	
232	/* Types from the Large File Support interface.  */
233	#ifndef __USE_FILE_OFFSET64
234	# ifndef __blkcnt_t_defined
235	typedef __blkcnt_t blkcnt_t;         /* Type to count number of disk blocks.  */
236	#  define __blkcnt_t_defined
237	# endif
238	# ifndef __fsblkcnt_t_defined
239	typedef __fsblkcnt_t fsblkcnt_t; /* Type to count file system blocks.  */
240	#  define __fsblkcnt_t_defined
241	# endif
242	# ifndef __fsfilcnt_t_defined
243	typedef __fsfilcnt_t fsfilcnt_t; /* Type to count file system inodes.  */
244	#  define __fsfilcnt_t_defined
245	# endif
246	#else
247	# ifndef __blkcnt_t_defined
248	typedef __blkcnt64_t blkcnt_t;           /* Type to count number of disk blocks.  */
249	#  define __blkcnt_t_defined
250	# endif
251	# ifndef __fsblkcnt_t_defined
252	typedef __fsblkcnt64_t fsblkcnt_t; /* Type to count file system blocks.  */
253	#  define __fsblkcnt_t_defined
254	# endif
255	# ifndef __fsfilcnt_t_defined
256	typedef __fsfilcnt64_t fsfilcnt_t; /* Type to count file system inodes.  */
257	#  define __fsfilcnt_t_defined
258	# endif
259	#endif
260	
261	#ifdef __USE_LARGEFILE64
262	typedef __blkcnt64_t blkcnt64_t;     /* Type to count number of disk blocks. */
263	typedef __fsblkcnt64_t fsblkcnt64_t; /* Type to count file system blocks.  */
264	typedef __fsfilcnt64_t fsfilcnt64_t; /* Type to count file system inodes.  */
265	#endif
266	
267	
268	/* Now add the thread types.  */
269	#if defined __USE_POSIX199506 || defined __USE_UNIX98
270	# include <bits/pthreadtypes.h>
271	#endif
272	
273	__END_DECLS
274	
275	#endif /* sys/types.h */
276	
