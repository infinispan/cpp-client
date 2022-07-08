FROM quay.io/centos/centos:stream8

ENV JAVA_HOME=/usr/lib/jvm/java-11

RUN yum -y install 'dnf-command(config-manager)'
RUN yum config-manager --set-enabled powertools
RUN dnf -y update && \
   dnf -y install \
   cmake \
   cyrus-sasl-devel \
   cyrus-sasl-plain \
   cyrus-sasl-md5 \
   cyrus-sasl-gssapi \
   drpm \
   iputils \
   java-11-openjdk-devel \
   krb5-workstation \
   maven \
   openssl-devel \
   procps-ng \
   protobuf \
   protobuf-devel \
   python3 \
   rpmdevtools \
   valgrind \
   wget \
   yum-utils && \
   dnf -y group install "Development Tools" && \
   dnf clean all && \
   rm -rf /var/cache/yum

#RUN wget http://repos.fedorapeople.org/repos/dchen/apache-maven/epel-apache-maven.repo -O /etc/yum.repos.d/epel-apache-maven.repo
#RUN yum -y install apache-maven


RUN groupadd -r jboss -g 1000 && useradd -u 1000 -r -g jboss -m -d /home/jboss -s /sbin/nologin -c "JBoss user" jboss && \
   chmod 755 /home/jboss

# Set the working directory to jboss' user home directory
WORKDIR /home/jboss

# Specify the user which should be used to execute all commands below
USER jboss


