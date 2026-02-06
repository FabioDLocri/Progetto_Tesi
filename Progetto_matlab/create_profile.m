
load('data/input/load_profile_raw.mat');    %[kW]
load('data/input/PV_irr_raw.mat');          %[W]

pv_profile_raw.date_time = datetime( pv_profile_raw.date_time,'InputFormat', 'yyyy-MM-dd HH:mm:ss');

pv_profile_raw_interp.t = (pv_profile_raw.date_time(1):minutes(1):pv_profile_raw.date_time(end))';
pv_profile_raw_interp.I= interp1(pv_profile_raw.date_time, pv_profile_raw.I, pv_profile_raw_interp.t, 'linear');

pv_pk = 20;
start_day = 180;   % giorno dell'anno
n_days    = 1;  % durata simulazione


t0 = pv_profile_raw_interp.t(1) + days(start_day - 1);
t1 = t0 + days(n_days) - minutes(1);

t2 = load_profile_raw.t(1) + days(start_day - 1);
t3 = t2 + days(n_days) - minutes(1);


idx1 = pv_profile_raw_interp.t >= t0 & pv_profile_raw_interp.t <= t1;
t_pvprofile = pv_profile_raw_interp.t(idx1);
i_pvprofile = pv_profile_raw_interp.I(idx1)*pv_pk *-1;
t_sec_pvprofile   = seconds(t_pvprofile - t_pvprofile(1));
pv_profile = [t_sec_pvprofile, i_pvprofile];

StopTime = t_sec_pvprofile(end);

idx2 = load_profile_raw.t >= t2 & load_profile_raw.t <= t3;
t_loadprofile = load_profile_raw.t(idx2);
i_loadprofile = load_profile_raw.P(idx2)* 1e3;
t_sec_loadprofile   = seconds(t_loadprofile - t_loadprofile(1));
load_profile = [t_sec_loadprofile, i_loadprofile];

figure; hold on;
plot(load_profile(:,1),load_profile(:,2));
plot(load_profile(:,1),pv_profile(:,2));
legend("load[W]", "PV[W]");
title('selected power profile');

