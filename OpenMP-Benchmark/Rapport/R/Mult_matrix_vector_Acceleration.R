#  M1 Informatique IM2AG-UGA
#  Parallel Algorithms and Programming - TP2:
#  Borne Jonathan Isnel Maxime
#  Résumé:
#  Etude statistique des performance d'éxecution d'un programme de Multiplication matrice/vecteur.
library(ggplot2)
library(plyr)
library(reshape2)

# extraction des données
d <- data.frame(
    read.csv2(file="./Mesures/mult_mat_1_16.csv",
              sep=';',
              dec='.')
)

filter_Algotype <- subset(d, Algotype==4)

# Temps moyen, écart type, variance
stat_raw<-ddply(
    d,
    c("Algotype", "Chunksize", "Nb_Thread"),
    summarise,
    N=length(Time),
    mean=mean(Time),
    sd=sd(Time),
    se=1.96*(sd/sqrt(N))
)
stat_raw

# On prends pour référence de temps d'execution séquentiel,
# le temps moyen (en nombre de cycles) calculé sur 1000 expériences d'éxécution  
# de l'algorithme naïf de multiplication entre une matrice triangulaire inférieure et un vecteur.
sequential_stat <- subset(stat_raw, Algotype==0 & Nb_Thread == 1 & Chunksize == 1);
mean_sequential_time <- sequential_stat$mean
mean_sequential_time

# On calcule dans le tableau "accel" l' accelération relative au temps séquentiel en fonction de la valeur
# du paramètre Chunksize pour chaque algorithme.
filter_stat <- subset(stat_raw, Algotype > 0)
accel <- data.frame(filter_stat)
#Calcul de l'accelération -> place le résultat dans la colonne mean de la table accel
accel$mean <- (1/accel$mean)*mean_sequential_time

# tracé des résultats obtenus

p1 <- ggplot(data = accel, mapping = aes(x=Nb_Thread, y=mean, color = as.factor(Chunksize))) +
    geom_point() +
    geom_line() +
    scale_x_continuous(breaks = seq(1,20)) +
    labs(
        colour = "Chunk Size",
        x = "Nombre de threads",
        y = "Acceleration"
    ) + facet_wrap(~Algotype) +
    theme_minimal()
ggsave("./Images/Accel.png", plot = p1, scale = 0.4)

show(p1)
