#  M1 Informatique IM2AG-UGA
#  Parallel Algorithms and Programming - TP2:
#  Borne Jonathan Isnel Maxime
#  Résumé:
#  Etude statistique des performance d'éxecution d'un programme de tri par bulles.
library(ggplot2)
library(plyr)
library(reshape2)

# extraction des données
d <- data.frame(
    read.csv2(file="./Mesures/bubble_1_16.csv",
              sep=';',
              dec='.')
)

filter_Algotype <- subset(d, Algo_type==1)

# Temps moyen, écart type, variance
stat_raw<-ddply(
    d,
    c("Algo_type", "Bloc_size", "Nb_threads"),
    summarise,
    N=length(Time),
    mean=mean(Time),
    sd=sd(Time),
    se=1.96*(sd/sqrt(N))
)
stat_raw

# On prends pour référence de temps d'execution,
# le temps moyen (en nombre de cycles) calculé sur 100 éxécutions  
# de l'algorithme de tri à bulles séquentiel.
sequential_stat <- subset(stat_raw, Algo_type==0 & Nb_threads == 1 & Bloc_size == 8);
mean_sequential_time <- sequential_stat$mean
mean_sequential_time

# On calcule dans le tableau "accel" l' accelération relative au temps séquentiel en fonction de la valeur
# du paramètre Bloc_size pour chaque algorithme et pour un nombre de thread fixé.
filter_stat <- subset(stat_raw, Algo_type > 0)
accel <- data.frame(filter_stat)
#Calcul de l'accelération -> place le résultat dans la colonne mean de la table accel
accel$mean <- (1/accel$mean)*mean_sequential_time

# tracé des résultats obtenus

p1 <- ggplot(data = accel, mapping = aes(x=Nb_threads, y=mean, color = as.factor(Bloc_size))) +
    geom_point() +
    geom_line() +
    scale_x_continuous(breaks = seq(1,20)) +
    labs(
        colour = "Bloc Size",
        x = "Nombre de threads",
        y = "Acceleration"
    ) + facet_wrap(~Algo_type) +
    theme_minimal()
ggsave("./Images/Bubble_acell.png", plot = p1, scale = 0.4)

show(p1)
