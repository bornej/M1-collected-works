#  M1 Informatique IM2AG-UGA
#  Cryptography - Siphash:
#  Borne Jonathan Duverney Thomas
#  Résumé:
#  Etude statistique de la recherche de collisions pour la fonction de hashage siphash.

library(ggplot2)
library(plyr)
library(reshape2)

# extraction des données
d <- data.frame(
    read.csv2(file="./Results/Collisions.csv",
              sep=';',
              dec='.')
)

# Nb_iteration moyen, écart type, variance
mean_iter<-ddply(
    d,
    c("run_id"),
    summarise,
    N=length(nb_iter),
    mean=mean(nb_iter),
    sd=sd(nb_iter),
    se=1.96*(sd/sqrt(N))
)

mean_iter

# Nb_iteration moyen, écart type, variance
mean_period<-ddply(
    d,
    c("run_id"),
    summarise,
    N=length(i-j),
    periode_moyenne=mean(i-j),
    sd=sd(i-j),
    se=1.96*(sd/sqrt(N))
)

mean_period

# tracé des résultats obtenu

p2 <- ggplot(d, aes(nb_iter)) +
    geom_histogram( binwidth = 10000) +
    theme_minimal()

ggsave("./Images/Collisions.png", plot = p2, scale = 1)

show(p2)
