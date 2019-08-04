#  M1 Informatique IM2AG-UGA
#  Parallel Algorithms and Programming - TP2:
#  Borne Jonathan Isnel Maxime
#  Résumé:
#  Etude statistique des performance d'éxecution d'un programme de tri par fusion.
library(ggplot2)
library(plyr)
library(reshape2)

# extraction des données
d <- data.frame(
    read.csv2(file="./Mesures/result_mergeSort_18threads_master_unnested_128_4096_100iter.csv",
              sep=';',
              dec='.')
)

# Temps moyen, écart type, variance
stat_raw<-ddply(
    d,
    c("Algotype", "Nb_Thread", "Taille_Tableau", "Threshold"),
    summarise,
    N=length(Time),
    mean=mean(Time),
    sd=sd(Time),
    se=1.96*(sd/sqrt(N))
)
#stat_raw

# On prends pour référence de temps d'execution,
# le temps moyen (en nombre de cycles) calculé sur 1000 éxécutions  
# de l'algorithme de tri à bulles séquentiel.
sequential_stat <- subset(stat_raw, Algotype==0);
sequential_stat
mean_sequential_time <- sequential_stat$mean
#mean_sequential_time

nrow(sequential_stat)

# On calcule dans le tableau "accel" l' accelération relative au temps séquentiel en fonction de la valeur
# du paramètre Bloc_size pour chaque algorithme et pour un nombre de thread fixé.
filter_stat <- subset(stat_raw, Algotype > 0)
accel <- data.frame(filter_stat)
nrow(accel)
#Calcul de l'accelération -> place le résultat dans la colonne mean de la table accel
accel$mean <- (1/accel$mean)*mean_sequential_time

# tracé des résultats obtenus

p1 <- ggplot(data = accel, mapping = aes(x=Nb_Thread, y=mean, color = as.factor(Taille_Tableau))) +
    geom_point() +
    geom_line() +
    scale_x_continuous(breaks = seq(1,24)) +
    labs(
        colour = "Taille tableau",
        x = "Nombre de threads",
        y = "Acceleration"
    ) + facet_wrap(~Threshold) +
    theme_minimal()
ggsave("./Images/Merge_acell_tmp.png", plot = p1, scale = 0.4)

show(p1)
