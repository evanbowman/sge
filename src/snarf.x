/* cpp arguments: SchemeInterface.cpp -std=c++14 -I /usr/local/include/guile/2.2/ */
scm_c_define_gsubr (s_EntityCreate, 0, 0, 0, (scm_t_subr) EntityCreate);;
scm_c_define_gsubr (s_EntityRemove, 1, 0, 0, (scm_t_subr) EntityRemove);;
scm_c_define_gsubr (s_EntitySetPosition, 3, 0, 0, (scm_t_subr) EntitySetPosition);;
scm_c_define_gsubr (s_EntityGetX, 1, 0, 0, (scm_t_subr) EntityGetX);;
scm_c_define_gsubr (s_EntityGetY, 1, 0, 0, (scm_t_subr) EntityGetY);;
scm_c_define_gsubr (s_IsRunning, 0, 0, 0, (scm_t_subr) IsRunning);;
scm_c_define_gsubr (s_TimerCreate, 0, 0, 0, (scm_t_subr) TimerCreate);;
scm_c_define_gsubr (s_TimerReset, 1, 0, 0, (scm_t_subr) TimerReset);;
scm_c_define_gsubr (s_TimerRemove, 1, 0, 0, (scm_t_subr) TimerRemove);;
scm_c_define_gsubr (s_CreateAnimation, 5, 0, 0, (scm_t_subr) CreateAnimation);;
