FROM centos:8

RUN yum -y install 'dnf-command(config-manager)'
RUN yum config-manager --set-enabled PowerTools
RUN yum -y install \
   wget \
   cmake \
   iputils \
   java-1.8.0-openjdk-devel \
   openssl-devel \
   rpmdevtools \
   tar \
   valgrind \
   maven \
   sudo \
   protobuf-devel \
   cyrus-sasl-devel \
   cyrus-sasl-plain \
   cyrus-sasl-md5 \
   cyrus-sasl-gssapi \
   krb5-workstation doxygen python3 && \
   yum -y groupinstall "Development Tools"

#RUN wget http://repos.fedorapeople.org/repos/dchen/apache-maven/epel-apache-maven.repo -O /etc/yum.repos.d/epel-apache-maven.repo
#RUN yum -y install apache-maven

RUN yum -y install which && \
   yum clean all

RUN useradd -ms /bin/bash jboss

RUN usermod -aG wheel jboss

RUN echo 'jboss ALL=(ALL) NOPASSWD:ALL' > /etc/sudoers

# Set the working directory to jboss' user home directory
WORKDIR /home/jboss

# Specify the user which should be used to execute all commands below
USER jboss


