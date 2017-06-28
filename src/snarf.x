/* cpp arguments: SchemeInterface.cpp -std=c++14 -I /usr/local/include/guile/2.2/ */
scm_c_define_gsubr (s_EntityCreate, 0, 0, 0, (scm_t_subr) EntityCreate);;
scm_c_define_gsubr (s_EntityRemove, 1, 0, 0, (scm_t_subr) EntityRemove);;
scm_c_define_gsubr (s_EntitySetPosition, 3, 0, 0, (scm_t_subr) EntitySetPosition);;
scm_c_define_gsubr (s_EntityGetPosition, 1, 0, 0, (scm_t_subr) EntityGetPosition);;
scm_c_define_gsubr (s_EntitySetAnimation, 2, 0, 0, (scm_t_subr) EntitySetAnimation);;
scm_c_define_gsubr (s_EntitySetKeyframe, 2, 0, 0, (scm_t_subr) EntitySetKeyframe);;
scm_c_define_gsubr (s_EntityGetKeyframe, 1, 0, 0, (scm_t_subr) EntityGetKeyframe);;
scm_c_define_gsubr (s_IsRunning, 0, 0, 0, (scm_t_subr) IsRunning);;
scm_c_define_gsubr (s_TimerCreate, 0, 0, 0, (scm_t_subr) TimerCreate);;
scm_c_define_gsubr (s_TimerReset, 1, 0, 0, (scm_t_subr) TimerReset);;
scm_c_define_gsubr (s_TimerRemove, 1, 0, 0, (scm_t_subr) TimerRemove);;
scm_c_define_gsubr (s_MicroSleep, 1, 0, 0, (scm_t_subr) MicroSleep);;
scm_c_define_gsubr (s_KeyPressed, 1, 0, 0, (scm_t_subr) KeyPressed);;
scm_c_define_gsubr (s_AnimationCreate, 7, 0, 0, (scm_t_subr) AnimationCreate);;
